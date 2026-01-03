#pragma once
#include <vector>
#include <iostream>
#include "Sorter.h"
#include "Test.h"
#include <string>

struct TestCase32 {
	using FuncPtr = void(*)(uint32_t*, size_t);
	FuncPtr function;
	std::string name;
	size_t max_size;  // 0 = без ограничений

	TestCase32(FuncPtr func, std::string n, size_t max = 0)
		: function(func), name(std::move(n)), max_size(max) {
	}
};

inline std::vector<TestCase32> get_all_tests_uint32() {
	return {
		// Медленные алгоритмы - маленькие лимиты
		/*{Sorter<uint32_t>::bubble_sort, "bubble", 100000},
		{Sorter<uint32_t>::bubble_sort_opt, "bubble_opt", 200000},
		{Sorter<uint32_t>::insertion_sort, "insertion", 500000},
		{Sorter<uint32_t>::insertion_sort_shift, "insertion_shift", 500000},
		{Sorter<uint32_t>::insertion_sort_bin, "insertion_bin", 1000000},
		{Sorter<uint32_t>::selection_sort, "selection", 500000},

		// Быстрые алгоритмы - без ограничений
		{Sorter<uint32_t>::shell_sort, "shell", 0},
		{Sorter<uint32_t>::shell_sort_knuth_arr, "shell_knuth_arr", 0},
		{Sorter<uint32_t>::shell_sort_knuth_func, "shell_knuth_func", 0},
		{Sorter<uint32_t>::shell_sort_hibbard_arr, "shell_hibbard_arr", 0},
		{Sorter<uint32_t>::shell_sort_hibbard_func, "shell_hibbard_func", 0},
		{Sorter<uint32_t>::shell_sort_sedgewick, "shell_sedgewick", 0},

		//алгоритмы второго ДЗ
		{Sorter<uint32_t>::selection_sort, "selection", 500000},
		{Sorter<uint32_t>::heap_sort, "heap sort", 0},*/

		//алгоритмы третьего ДЗ
		{Sorter<uint32_t>::quick_sort, "quick_sort", 0},
		{Sorter<uint32_t>::quick_sort_optimized, "quick_sort_opt", 0},		
		{Sorter<uint32_t>::quick_sort_safe, "quick_sort_safe", 0},
		{Sorter<uint32_t>::merge_sort, "merge_sort", 0}
	};
}

// Для uint8_t (digits)
struct TestCase8 {
	using FuncPtr = void(*)(uint8_t*, size_t);
	FuncPtr function;
	std::string name;
	size_t max_size;

	TestCase8(FuncPtr func, std::string n, size_t max = 0)
		: function(func), name(std::move(n)), max_size(max) {
	}
};

inline std::vector<TestCase8> get_all_tests_uint8() {
	return {
		// Для uint8_t можно больше - меньше памяти
		/*{Sorter<uint8_t>::bubble_sort, "bubble", 200000},
		{Sorter<uint8_t>::bubble_sort_opt, "bubble_opt", 500000},
		{Sorter<uint8_t>::insertion_sort, "insertion", 500000},
		{Sorter<uint8_t>::insertion_sort_shift, "insertion_shift", 500000},
		{Sorter<uint8_t>::insertion_sort_bin, "insertion_bin", 1000000},
		{Sorter<uint8_t>::selection_sort, "selection", 500000},

		// Быстрые алгоритмы - без ограничений
		{Sorter<uint8_t>::shell_sort, "shell", 0},
		{Sorter<uint8_t>::shell_sort_knuth_arr, "shell_knuth_arr", 0},
		{Sorter<uint8_t>::shell_sort_knuth_func, "shell_knuth_func", 0},
		{Sorter<uint8_t>::shell_sort_hibbard_arr, "shell_hibbard_arr", 0},
		{Sorter<uint8_t>::shell_sort_hibbard_func, "shell_hibbard_func", 0},
		{Sorter<uint8_t>::shell_sort_sedgewick, "shell_sedgewick", 0},

		//алгоритмы второго ДЗ
		{Sorter<uint8_t>::selection_sort, "selection", 500000},
		{Sorter<uint8_t>::heap_sort, "heap sort", 0}, */

		//алгоритмы третьего ДЗ
		{Sorter<uint8_t>::quick_sort, "quick_sort", 99999},
		{Sorter<uint8_t>::quick_sort_optimized, "quick_sort_opt", 99999},
		{Sorter<uint8_t>::quick_sort_safe, "quick_sort_safe", 0},
		{Sorter<uint8_t>::merge_sort, "merge_sort", 0}

	};

};
