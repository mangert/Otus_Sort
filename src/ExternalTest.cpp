#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <exception>
#include <vector>
#include <functional>
#include "ExternalSorter.cpp"

class ExternalTest {

public:    
    ExternalTest() {
        cases_N = { 100, 1000, 10000, 100000, 1000000 };        
    }
    
    // Тестирование одного случая
    void test(size_t N, uint32_t T, std::function<void(const std::string&,
        const std::string&, size_t, const std::string&)> sort_func, const std::string& info) {
        
        std::string input_file = "test_input_" + std::to_string(N) + "_" +
            std::to_string(T) + ".txt";
        std::string output_file = "test_output_" + std::to_string(N) + "_" +
            std::to_string(T) + ".txt";        

        try {
            // Генерация тестового файла
            std::cout << "\n=== Testing " << info<< ": N = " << N << ", T = " << T << " == = \n";
            
            std::cout << "\nTest data generating...";
            ExternalSorter::generate_file(input_file, N, T);
            std::cout << "\rData generaiting completed... Sorting..." << std::flush;

            // Замер времени
            auto start = std::chrono::high_resolution_clock::now();

            sort_func(input_file, output_file, T, "temp_buckets");

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "\rSorting completed in " << duration.count() << " ms. Vefifying..." << std::flush;
            std::cout << "\n";
            // Проверка результата             
            if (verify_sorted_file(output_file)) {
                std::cout << "\nSUCCESS: Sorted " << N << " numbers in "
                    << duration.count() << " ms\n";
            }
            else {
                std::cout << "\nFAILED: Sorting verification failed\n";
            }

            // Очистка тестовых файлов
            namespace fs = std::filesystem;
            fs::remove(input_file);
            fs::remove(output_file);

        }
        catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << "\n";
        }
        std::cout << "\n==========================\n";
    }

    // Запуск тестов для всех случаев
    void run_all_tests(std::function<void(const std::string&,
        const std::string&, size_t, const std::string&)> sort_func, 
        size_t max_T, const std::string& test_info) {

        std::cout << test_info << " Performance Test\n";
        std::cout << "N\tT=10\tT=N\n";

        for (size_t N : cases_N) {           

            // T = 10
            try {
                test(N, 10, sort_func, test_info);
            }
            catch (...) {
                std::cout << "FAIL\t";
            }

            // T = N            
            try {
                if (N > max_T) {
                    std::cout << "Test SKIPPED - too large data\t";
                    continue;
                };
                test(N, N, sort_func, test_info);
            }
            catch (...) {
                std::cout << "FAIL\n";
            }
        }
    }

private:    
    static bool verify_sorted_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open file for verification: " << filename << "\n";
            return false;
        }

        std::string line;
        uint32_t prev = 0;
        uint32_t current;
        size_t line_num = 0;
        bool first_line = true;

        while (std::getline(file, line)) {
            // Пропускаем пустые строки
            if (line.empty()) continue;

            try {
                current = std::stoul(line);
            }
            catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid number at line " << (line_num + 1)
                    << ": '" << line << "' - " << e.what() << "\n";
                return false;
            }

            if (!first_line && current < prev) {
                std::cerr << "ERROR: Sorting violation at line " << (line_num + 1)
                    << ": " << prev << " > " << current << "\n";
                return false;
            }

            prev = current;
            first_line = false;
            line_num++;

            // Периодический вывод прогресса для больших файлов
            if (line_num % 1000000 == 0) {
                std::cout << "  Verified " << line_num << " lines...\n";
            }
        }

        if (line_num == 0) {
            std::cerr << "WARNING: Empty file: " << filename << "\n";
        }

        std::cout << "File " << filename << " is correctly sorted ("
            << line_num << " numbers)\n";
        return true;
    };

private:
    std::vector<size_t> cases_N;
};