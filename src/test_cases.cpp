#pragma once
#include <vector>
#include <iostream>
#include "Sorter.h"
#include "Test.h"
#include <string>
#include "LinearSorter.h"
#include "BinaryTest.cpp"

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
		{Sorter<uint32_t>::bubble_sort, "bubble", 100000},
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
		{Sorter<uint32_t>::heap_sort, "heap sort", 0},

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
		{Sorter<uint8_t>::bubble_sort, "bubble", 200000},
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
		{Sorter<uint8_t>::heap_sort, "heap sort", 0},

		//алгоритмы третьего ДЗ
		{Sorter<uint8_t>::quick_sort, "quick_sort", 99999},
		{Sorter<uint8_t>::quick_sort_optimized, "quick_sort_opt", 99999},
		{Sorter<uint8_t>::quick_sort_safe, "quick_sort_safe", 0},
		{Sorter<uint8_t>::merge_sort, "merge_sort", 0}

	};

};

struct TestCaseLinear {
	using FuncPtr = void(*)(int16_t*, size_t);
	FuncPtr function;
	std::string info;

	TestCaseLinear(FuncPtr func, std::string info)
		: function(func), info(std::move(info)) {
	};
};

inline std::vector<TestCaseLinear> get_all_tests_linear() {
	return {

		{LinearSorter<int16_t>::bucket_sort, "bucket sort"},
		{LinearSorter<int16_t>::bucket_sort_linear, "bucket sort linear"},
		{LinearSorter<int16_t>::bucket_sort_sqrt, "bucket sort sqrt"},
		{LinearSorter<int16_t>::counting_sort, "counting sort"},
		{[](int16_t* data, size_t size) {LinearSorter<int16_t>::radix_sort(data, size, 10); },
			"radix sort with base 10"},
		{[](int16_t* data, size_t size) {LinearSorter<int16_t>::radix_sort(data, size, 2); },
			"radix sort with base 2"},
		{[](int16_t* data, size_t size) {LinearSorter<int16_t>::radix_sort(data, size, 2); },
			"radix sort with base 8"},
	};

};

inline std::vector<BinaryTest::TestCase> get_binary_tests() {
	
	std::vector<BinaryTest::TestCase> test_cases;

	// CountingSort
	test_cases.push_back({
		[](std::string& input, std::string& output, uint16_t) {
			BinaryFileSorter::counting_sort(input, output, std::numeric_limits<uint16_t>::max());
		},
		"CountingSort"
		});

	// RadixSort base=256
	test_cases.push_back({
		[](std::string& input, std::string& output, uint16_t) {
			BinaryFileSorter::radix_sort(input, output, std::numeric_limits<uint16_t>::max(), 256);
		},
		"RadixSort (base=256)",
		100000000
		});

	// RadixSort base=128
	test_cases.push_back({
		[](std::string& input, std::string& output, uint16_t) {
			BinaryFileSorter::radix_sort(input, output, std::numeric_limits<uint16_t>::max(), 128);
		},
		"RadixSort (base=128)",
		100000000
		});


	// BucketSort с 256 корзинами
	test_cases.push_back({
		[](std::string& input, std::string& output, uint16_t) {
			BinaryFileSorter::bucket_sort(input, output, std::numeric_limits<uint16_t>::max(), 256);
		},
		"BucketSort256",
		100000000
		});
	
	return test_cases;

};