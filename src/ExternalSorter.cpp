#pragma once
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <random>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <exception>


class ExternalSorter {
    
public:

    // ES1: Создание T файлов с последующим слиянием (Bucket Sort)
    static void external_sort_method1(const std::string& input_file,
        const std::string& output_file,
        size_t T,
        const std::string& temp_dir = "temp_buckets") {

        // 0. Создаём директорию для временных файлов
        create_temp_directory(temp_dir);

        try {
            // 1. Распределяем числа по bucket-файлам
            distribute_to_buckets(input_file, temp_dir, T);

            // 2. Объединяем bucket-файлы в выходной файл
            merge_buckets(output_file, temp_dir, T);

            // 3. Очищаем временную директорию
            cleanup_temp_directory(temp_dir);

        }
        catch (const std::exception& e) {
            // В случае ошибки пытаемся очистить временные файлы
            try {
                cleanup_temp_directory(temp_dir);
            }
            catch (...) {
                // Игнорируем ошибки при очистке
            }
            throw;  // Пробрасываем оригинальную ошибку
        }
    }
    
    //функция записи случайных чисел в файл
    static void generate_file(const std::string& filename,
		size_t n, uint32_t lim) {

        if (lim == 0) {
            std::cerr << "ERROR: lim must be > 0\n";
            return;
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "  WARNING: Cannot open result file: " << filename << "\n";
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());  
        std::uniform_int_distribution<uint32_t> distrib(1, lim);

        // Записываем случайные числа
        for (size_t i = 0; i != n; ++i) {
            file << distrib(gen);
            if (i != n - 1) {
                file << '\n';
            }
        }

        if (!file) {
            std::cerr << "ERROR: Write failed for file: " << filename << "\n";
        }
	}
private:
    
    // Создание временной директории
    static void create_temp_directory(const std::string& temp_dir) {
        namespace fs = std::filesystem;
        if (fs::exists(temp_dir)) {
            // Очищаем, если уже существует
            fs::remove_all(temp_dir);
        }

        if (!fs::create_directory(temp_dir)) {
            throw std::runtime_error("Cannot create temp directory: " + temp_dir);
        }

        std::cout << "Created temp directory: " << fs::absolute(temp_dir) << "\n";
    }

    // Распределение чисел по bucket-файлам
    static void distribute_to_buckets(const std::string& input_file,
        const std::string& temp_dir,
        size_t T) {

        std::ifstream input(input_file);
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open input file: " + input_file);
        }

        std::cout << "Distributing numbers to " << T << " buckets...\n";

        // Буферы для часто встречающихся ключей (оптимизация)
        const size_t BUFFER_LIMIT = 10000;  // Число строк в буфере
        std::unordered_map<size_t, std::string> buffers;
        size_t total_numbers = 0;

        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) continue;

            // Преобразуем строку в число
            size_t key;
            try {
                key = std::stoul(line);
            }
            catch (const std::exception& e) {
                throw std::runtime_error("Invalid number in input file: " + line);
            }

            // Проверяем диапазон
            if (key < 1 || key > T) {
                throw std::runtime_error("Number " + std::to_string(key) +
                    " out of range [1, " + std::to_string(T) + "]");
            }

            // Добавляем в буфер для этого ключа
            buffers[key].append(line).append("\n");
            total_numbers++;

            // Если буфер заполнен - сбрасываем в файл
            if (buffers[key].size() > BUFFER_LIMIT * 10) {  // Примерный расчёт
                append_to_bucket_file(temp_dir, key, buffers[key]);
                buffers[key].clear();
            }
        }

        // Сбрасываем остатки буферов в файлы
        for (auto& [key, buffer] : buffers) {
            if (!buffer.empty()) {
                append_to_bucket_file(temp_dir, key, buffer);
            }
        }

        std::cout << "  Processed " << total_numbers << " numbers\n";
        input.close();
    }
    // Добавление данных в bucket-файл
    static void append_to_bucket_file(const std::string& temp_dir,
        size_t bucket_key,
        const std::string& data) {

        std::string filename = get_bucket_filename(temp_dir, bucket_key);
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot open bucket file: " + filename);
        }

        file << data;

        if (!file) {
            throw std::runtime_error("Write failed to bucket file: " + filename);
        }
    }

    // Получение имени bucket-файла
    static std::string get_bucket_filename(const std::string& temp_dir,
        size_t bucket_key) {
        return temp_dir + "/bucket_" + std::to_string(bucket_key) + ".txt";
    }

    // Объединение bucket-файлов в выходной файл
    static void merge_buckets(const std::string& output_file,
        const std::string& temp_dir,
        size_t T) {

        std::ofstream output(output_file);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot open output file: " + output_file);
        }

        std::cout << "Merging " << T << " buckets to output...\n";

        size_t total_written = 0;
        namespace fs = std::filesystem;
        for (size_t key = 1; key <= T; ++key) {
            std::string filename = get_bucket_filename(temp_dir, key);

            if (fs::exists(filename)) {
                std::ifstream bucket_file(filename);
                if (!bucket_file.is_open()) {
                    std::cerr << "Warning: Cannot open bucket file: " << filename << "\n";
                    continue;
                }

                std::string line;
                while (std::getline(bucket_file, line)) {
                    if (!line.empty()) {
                        output << line << "\n";
                        total_written++;

                        // Периодически сбрасываем буфер
                        if (total_written % 100000 == 0) {
                            output.flush();
                        }
                    }
                }

                bucket_file.close();
            }
            // Если файла нет (нет чисел с таким key) - просто пропускаем
        }

        output.close();
        std::cout << "  Written " << total_written << " numbers to output\n";
    }

    // Очистка временной директории
    static void cleanup_temp_directory(const std::string& temp_dir) {
        namespace fs = std::filesystem;
        if (fs::exists(temp_dir)) {
            try {
                fs::remove_all(temp_dir);
                std::cout << "Cleaned up temp directory: " << temp_dir << "\n";
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Warning: Failed to cleanup temp directory: "
                    << e.what() << "\n";
            }
        }
    }

};


