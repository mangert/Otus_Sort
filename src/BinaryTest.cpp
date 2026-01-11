#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <exception>
#include <vector>
#include <functional>
#include "BinaryFileSorter.cpp"

class BinaryTest {

public:    
    
    using SortFunction = std::function<void(std::string&, std::string&, uint16_t)>;

    struct TestCase {
        SortFunction func;
        std::string name;        
    };

    BinaryTest(std::vector<TestCase> test_cases)
        : test_cases_(std::move(test_cases)) {
    }

    // Полная программа тестирования
    void test(size_t size) {
        namespace fs = std::filesystem;
        std::cout << "\n=== Подготовка - генерация тестового файла ===\n";
        std::string input_file = "input_data.bin";

        try {
            // Генерация тестового файла
            auto start = std::chrono::high_resolution_clock::now();
            
            BinaryFileSorter::generate_random_file(size, input_file);
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "\n= Сгенерирован файл " << input_file << 
                " из " << size << " чисел " << " за " << duration.count() << "ms \n";
        
            // Тестирование всех случаев
            for (auto& test_case: test_cases_) {
                std::cout << "\n=== Запуск: " << test_case.name << " ===\n";
                std::string output_file = test_case.name + ".bin";
                start = std::chrono::high_resolution_clock::now();
                test_case.func(input_file, output_file, std::numeric_limits<uint16_t>::max());
                end = std::chrono::high_resolution_clock::now();
                duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                
                if (verify_sorted_file(output_file, false)) { // false = без прогресса
                    std::cout << "\nSUCCESS: файл отсортирован за " << duration.count() << " ms\n";
                }
                else {
                    std::cout << "\nFAILED: файл НЕ отсортирован правильно!\n";
                }
                
                fs::remove(output_file);
            }

            // Очистка тестовых файлов            
            fs::remove(input_file);
            //fs::remove(output_files);

        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
        }
        std::cout << "\n==========================\n";
    }

    
private:
    static bool verify_sorted_file(const std::string& filename, bool show_progress = true) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Ошибка открытия файла для проверки: " << filename << std::endl;
            return false;
        }

        // Определяем размер файла
        file.seekg(0, std::ios::end);
        uint64_t file_size = file.tellg();
        uint64_t total_numbers = file_size / sizeof(uint16_t);
        file.seekg(0, std::ios::beg);

        if (total_numbers == 0) {
            std::cout << "Файл пустой" << std::endl;
            return true;
        }

        std::cout << "Проверка сортировки файла: " << filename
            << " (" << total_numbers << " чисел)" << std::endl;

        // Буферизованное чтение
        const size_t BUFFER_SIZE = 1024 * 1024; // 1 МБ чисел
        std::vector<char> byte_buffer(BUFFER_SIZE * sizeof(uint16_t));

        uint16_t prev, current;
        uint64_t numbers_checked = 0;
        bool is_sorted = true;

        // Читаем первое число отдельно
        if (!file.read(reinterpret_cast<char*>(&prev), sizeof(prev))) {
            std::cerr << "Не могу прочитать первое число" << std::endl;
            return false;
        }
        numbers_checked++;

        while (file) {
            file.read(byte_buffer.data(), byte_buffer.size());
            size_t bytes_read = file.gcount();
            if (bytes_read == 0) break;

            size_t numbers_in_buffer = bytes_read / sizeof(uint16_t);

            // Проверяем числа в буфере
            for (size_t i = 0; i < numbers_in_buffer; ++i) {
                const char* byte_ptr = byte_buffer.data() + i * sizeof(uint16_t);
                current = read_uint16_LE(byte_ptr);

                if (prev > current) {
                    std::cout << "\nНАЙДЕНА ОШИБКА СОРТИРОВКИ!" << std::endl;
                    std::cout << "  Позиция: " << numbers_checked << "-" << (numbers_checked + 1)
                        << " чисел" << std::endl;
                    std::cout << "  Предыдущее: " << prev << " > Текущее: " << current << std::endl;
                    is_sorted = false;
                    break;
                }

                prev = current;
                numbers_checked++;
            }

            // Вывод прогресса для больших файлов
            if (show_progress && numbers_checked % (10 * 1024 * 1024) == 0) {
                double percent = (numbers_checked * 100.0) / total_numbers;
                std::cout << "  Прогресс: " << std::fixed << std::setprecision(1)
                    << percent << "% (" << numbers_checked / 1000000
                    << " млн чисел)" << std::endl;
            }

            if (!is_sorted) break;
        }

        file.close();

        if (is_sorted) {
            std::cout << "? Файл корректно отсортирован! Проверено "
                << numbers_checked << " чисел." << std::endl;
        }
        else {
            std::cout << "? Файл НЕ отсортирован корректно!" << std::endl;
        }

        return is_sorted;
    }

    static inline uint16_t read_uint16_LE(const char* bytes) {
        // Little Endian: младший байт первый
        return static_cast<uint16_t>(
            static_cast<uint8_t>(bytes[0]) |
            (static_cast<uint8_t>(bytes[1]) << 8)
            );
    }

private:
    
    std::vector<TestCase> test_cases_;
};