#pragma once
#include <iostream>
#include <concepts>
#include <random>

//вспомогательные функции для заполнения и вывода в консоль целочисленных массивов
namespace data_generator {	
	
	template <std::integral T> //последовательность от 0 до n
	void fill_sequenced(T* const array, size_t n) {
		
		for (size_t i = 0; i != n; ++i) {
			array[i] = i;
		};		
	};

	template <std::integral T> //последовательность от 0 до n
	void fill_reversed(T* const array, size_t n) {

		for (size_t i = n; i > 0; --i) {
			array[n - i] = i;
		};
	};

	template <std::integral T> //случайный набор в диапазоне от 0 до заданного максимума
	void fill_random(T* const array, size_t n, T max_val) {
		std::random_device rd;
		std::mt19937_64 gen(rd());  // 64-битная версия
		std::uniform_int_distribution<uint64_t> distrib;
		
		for (size_t i = 0; i != n; ++i) {
			array[i] = static_cast<T>((distrib(gen)) % max_val);
		};		
	};
	
	template <std::integral T>
	void print_array(T* const array, size_t n) {
		for (size_t i = 0; i != n; ++i)
			std::cout << array[i] << " ";
		std::cout << "\n";
	};
}