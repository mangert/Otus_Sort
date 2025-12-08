#include <iostream>
#include "generator.cpp"
#include "Sorter.h"
#include "Test.h"

int main() {
	
	//Установки программы тестирования
	//набор папок
	std::string test_folders[] = {"0.random", "1.digits", "2.sorted", "3.revers"};	
	
	//набор функций
	using FuncPtr = void(*)(uint32_t*, size_t); //для всех тестов, кроме digits
	FuncPtr sort_funcs_all[10] = {
		Sorter<uint32_t>::insertion_sort_shift,
		Sorter<uint32_t>::insertion_sort_bin,
		Sorter<uint32_t>::shell_sort,
		Sorter<uint32_t>::shell_sort_knuth_arr,
		Sorter<uint32_t>::shell_sort_knuth_func,
		Sorter<uint32_t>::shell_sort_hibbard_arr,
		Sorter<uint32_t>::shell_sort_hibbard_func,
		Sorter<uint32_t>::shell_sort_sedgewick,
		Sorter<uint32_t>::bubble_sort,
		Sorter<uint32_t>::bubble_sort_opt,
	};
	
	using FuncPtrDig = void(*)(uint8_t*, size_t); //только для папки digits
	FuncPtrDig sort_funcs_dig[10] = {
		Sorter<uint8_t>::insertion_sort_shift,
		Sorter<uint8_t>::insertion_sort_bin,
		Sorter<uint8_t>::shell_sort,
		Sorter<uint8_t>::shell_sort_knuth_arr,
		Sorter<uint8_t>::shell_sort_knuth_func,
		Sorter<uint8_t>::shell_sort_hibbard_arr,
		Sorter<uint8_t>::shell_sort_hibbard_func,
		Sorter<uint8_t>::shell_sort_sedgewick,
		Sorter<uint8_t>::bubble_sort,
		Sorter<uint8_t>::bubble_sort_opt,
	};	
	for (auto& folder : test_folders) {
		
		for (size_t func = 3; func != 10; ++func) {
			std::cout << "----------- " << func << " ---------------" << std::endl;
			int p = 0;
			if (func < 8) p = 2;
			if (folder == test_folders[2]) {
				Test<uint8_t> test(sort_funcs_dig[func], folder, Test<uint8_t>::SkipPolicy(p));
				test.run_all();
			}
			else {
				Test<uint32_t> test(sort_funcs_all[func], folder, Test<uint32_t>::SkipPolicy(p));
				test.run_all();
			}
			std::cout << "----------- end " << func << " ---------------" << std::endl;
		}	
		
	};
	
	/*Test<uint32_t> test([](uint32_t* data, size_t size) {
		Sorter<uint32_t>::bubble_sort(data, size);
		}, "1.digits");
	Test<uint32_t> test(Sorter<uint32_t>::shell_sort_sedgewick, "1.digits");
	test.run_all();*/
	/*uint64_t rnd_arr[10];
	data_generator::fill_random<uint64_t>(rnd_arr, 10, 20);
	data_generator::print_array<uint64_t>(rnd_arr, 10);

	std::cout << std::endl;	
	Sorter<uint64_t>::shell_sort_sedgewick(rnd_arr, 10);

	data_generator::print_array<uint64_t>(rnd_arr, 10);

	int arr1[] = { 11, 16, 4, 3, 12, 0, 5, 8, 3, 7 };
	int arr2[] = { 11, 16, 4, 3, 12, 0, 5, 8, 3, 7 };
	int arr3[] = { 11, 16, 4, 3, 12, 0, 5, 8, 3, 7 };

	Sorter<int>::shell_sort_sedgewick(arr1, 10);
	Sorter<int>::shell_sort_knuth_arr(arr2, 10);
	Sorter<int>::shell_sort_hibbard_arr(arr3, 10);
	data_generator::print_array<int>(arr1, 10);
	data_generator::print_array<int>(arr2, 10);
	data_generator::print_array<int>(arr3, 10);
	*/
	return 0; 
}