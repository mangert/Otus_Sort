#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <random>

template <std::integral T>
class LinearTest {
    static_assert(std::is_integral_v<T>, "Test only works with integral types");

public:
    using FuncPtr = void(*)(T*, size_t);  // сигнатура функции сортировки

    LinearTest(FuncPtr test_run) : test_run(test_run) {};
    LinearTest(T min, T max, FuncPtr test_run) 
        : min_val(min), max_val(max), test_run(test_run) {};

    void run_all(std::string sort_info) {        

        std::cout << "\n========== Testing: " << sort_info << " ==========\n";
        std::cout << "\nmin_value: " << min_val << " max_value: " << max_val << "\n";

        for (int i = 0; i != test_cases.size(); ++i) {
            auto data = generate_data(min_val, max_val, test_cases[i]);            
            run_test(i, data.get(), test_cases[i]);            
        }
        std::cout << "========== End testing ==========\n\n";
    }

private:
    //генерируем массив случайных чисел в заданном диапазоне
    std::shared_ptr<T[]> generate_data(T min, T max, size_t size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> distrib(min, max);

        std::shared_ptr<T[]> data(new T[size]);
        for (size_t i = 0; i != size; ++i) {
            data[i] = distrib(gen);
        };

        return data;
    };

    //запуск одного теста
    void run_test(int test_num, T* data, size_t size) {

        //сохраняем исходный массив для проверки
        std::unique_ptr<T[]> _original(new T[size]);
        T* original = _original.get();
        std::copy_n(data, size, original);
        
        std::cout << "Test " << test_num << ": ";

        try {            
            //Запуск сортировки с замером времени
            auto start_time = std::chrono::high_resolution_clock::now();
            test_run(data, size);
            auto end_time = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                end_time - start_time);

            //Проверка результата
            bool is_correct = verify_result(original, data, size);

            std::cout << (is_correct ? "PASS" : "FAIL")
                << " | Size: " << size
                << " | Time: " << duration.count() << " ms\n";
            
        }
        catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }

    //проверка, что отсортировалось и данные не испортились
    bool verify_result(T* original, T* sorted, size_t size) {
        // Быстрая проверка что отсортировано
        if (!std::is_sorted(sorted, sorted + size)) return false;

        // Для маленьких массивов - полная проверка
        if (size <= 10000) {
            std::vector<T> copy(original, original + size);
            std::sort(copy.begin(), copy.end());
            return std::equal(sorted, sorted + size, copy.begin());
        }

        // Для больших - только сумма (быстрее)
        T sum_o = 0, sum_s = 0;
        for (size_t i = 0; i < size; ++i) {
            sum_o += original[i];
            sum_s += sorted[i];
        }
        return sum_o == sum_s;
    }
    

private:
    FuncPtr test_run; //функция для тестирования
    std::vector<size_t> test_cases = { 100, 1000, 10000, 100000, 1000000};    
    T min_val = 0;    
    T max_val = 999;
};