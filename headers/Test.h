#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <functional>
#include <memory>
#include <charconv>
#include <algorithm>
#include <type_traits>

template <typename T>
class Test {
    static_assert(std::is_integral_v<T>, "Test only works with integral types");    

public:
    using FuncPtr = void(*)(T*, size_t);  // сигнатура функции сортировки

    Test(FuncPtr run, std::string folder)
        : test_run(run), folder(folder) {
    };    

    void run_all(size_t max_supported_size = 0) {
        namespace fs = std::filesystem;

        std::cout << "\n========== Testing: " << folder << " ==========\n";

        for (int i = 0; ; ++i) {
            std::string input_file = get_test_path(i, "in");
            std::string output_file = get_test_path(i, "out");

            if (!fs::exists(input_file) || !fs::exists(output_file)) {
                if (i == 0) {
                    std::cerr << "No test files in: " << folder << "\n";
                }
                break;
            }

            run_test(i, input_file, output_file, max_supported_size);
        }

        std::cout << "========== End testing ==========\n\n";
    }

private:
    struct TestData {
        size_t size;
        std::unique_ptr<T[]> data;
    };

    void run_test(int test_num, const std::string& input_file,
        const std::string& output_file, size_t max_supported_size = 0) {
        
        std::cout << "Test " << test_num << ": ";

        try {
            // 1. Загрузка данных
            auto [size, input_data] = load_test_data(input_file, max_supported_size);
            
            if (!input_data) { //если тест нужно пропустить...
                std::cout << "Test " << test_num << ": SKIPPED (size=" << size << ")\n";
                return;
            };
            // 2. Копия для сортировки
            auto data_to_sort = std::make_unique<T[]>(size);
            std::copy(input_data.get(), input_data.get() + size, data_to_sort.get());

            // 3. Запуск сортировки с замером времени
            auto start_time = std::chrono::high_resolution_clock::now();
            test_run(data_to_sort.get(), size);
            auto end_time = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time);

            // 4. Проверка результата
            bool is_correct = verify_result(output_file, data_to_sort.get(), size);

            std::cout << (is_correct ? "PASS" : "FAIL")
                << " | Size: " << size
                << " | Time: " << duration.count() << " ms\n";

            // Дополнительная проверка для больших тестов
            if (size > 100000000) {
                bool is_sorted = std::is_sorted(data_to_sort.get(),
                    data_to_sort.get() + size);
                if (!is_sorted) {
                    std::cerr << "  WARNING: Large array is not properly sorted!\n";
                }
            }

        }
        catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }

    TestData load_test_data(const std::string& filename, size_t max_supported_size) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open: " + filename);
        }

        // 1. Читаем размер
        std::string size_line;
        std::getline(file, size_line);

        size_t size;
        auto [ptr, ec] = std::from_chars(size_line.data(),
            size_line.data() + size_line.size(),
            size);
        if (ec != std::errc()) {
            throw std::runtime_error("Invalid size in: " + filename);
        }

        //если размер данных превышает заданную для теста границу, данные не забираем (тест будет пропущен)
        if (max_supported_size > 0 && (size > max_supported_size)) {
            return { size, nullptr};
        };

        // 2. Читаем данные
        std::string data_line;
        if (!std::getline(file, data_line)) {
            throw std::runtime_error("No data in: " + filename);
        }

        // 3. Выделяем память (с проверкой на переполнение)
        if (size > SIZE_MAX / sizeof(T)) {
            throw std::runtime_error("Array too large: " + std::to_string(size));
        }

        auto data = std::make_unique<T[]>(size);

        // 4. Парсим строку
        const char* str = data_line.c_str();
        const char* end = str + data_line.size();
        size_t count = 0;

        while (str < end && count < size) {
            // Пропускаем пробелы
            while (str < end && (*str == ' ' || *str == '\t')) ++str;
            if (str >= end) break;

            // Парсим число как uint64, потом конвертируем в T
            uint64_t value;
            auto [next, err] = std::from_chars(str, end, value);

            if (err != std::errc()) {
                throw std::runtime_error("Parse error at position " +
                    std::to_string(str - data_line.c_str()));
            }

            // Проверяем, что число влезает в T
            if (value > static_cast<uint64_t>(std::numeric_limits<T>::max())) {
                throw std::runtime_error("Value " + std::to_string(value) +
                    " exceeds type limit");
            }

            data[count++] = static_cast<T>(value);
            str = next;
        }

        if (count != size) {
            throw std::runtime_error("Expected " + std::to_string(size) +
                " numbers, got " + std::to_string(count));
        }

        return { size, std::move(data) };
    }

    bool verify_result(const std::string& filename, const T* sorted_data, size_t size) {
        // Для очень больших тестов просто проверяем сортировку
        if (size > 100000000) {
            return std::is_sorted(sorted_data, sorted_data + size);
        }

        // Для обычных тестов сравниваем с файлом
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open: " + filename);
        }

        std::string line;
        if (!std::getline(file, line)) {
            throw std::runtime_error("Empty file: " + filename);
        }

        const char* str = line.c_str();
        const char* end = str + line.size();
        size_t count = 0;

        while (str < end && count < size) {
            while (str < end && (*str == ' ' || *str == '\t')) ++str;
            if (str >= end) break;

            uint64_t expected;
            auto [next, err] = std::from_chars(str, end, expected);

            if (err != std::errc()) {
                throw std::runtime_error("Parse error in output file");
            }

            if (static_cast<uint64_t>(sorted_data[count]) != expected) {
                std::cerr << "\n  Mismatch at position " << count
                    << ": expected " << expected
                    << ", got " << static_cast<uint64_t>(sorted_data[count]) << "\n";
                return false;
            }

            ++count;
            str = next;
        }

        return (count == size);
    }

    std::string get_test_path(int test_num, const std::string& extension) {
        return std::string(TEST_DIR) + "/" + folder + "/test." +
            std::to_string(test_num) + "." + extension;
    }

private:    
    FuncPtr test_run; //функция для тестирования
    std::string folder; //папка с тестами    
};
