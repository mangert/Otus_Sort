#include <iostream>
#include "generator.cpp"
#include "Sorter.h"
#include "Test.h"
#include "test_cases.cpp"

int main() {
	// Получаем все тестовые случаи
	auto test_cases_32 = get_all_tests_uint32();
	auto test_cases_8 = get_all_tests_uint8();

	std::string folders[] = { "0.random", "1.digits", "2.sorted", "3.revers" };

	// Проходим по алгоритмам
	for (const auto& test_case : test_cases_32) {
		std::cout << "\n\n=========================================\n";
		std::cout << "ALGORITHM: " << test_case.name << "\n";
		std::cout << "=========================================\n";

		// Проходим по папкам (кроме digits)
		for (const auto& folder : folders) {
			if (folder == "1.digits") continue;  // digits тестируем отдельно

			std::cout << "\n--- Data type: " << folder << " ---\n";

			Test<uint32_t> test(test_case.function, folder);
			test.run_all(test_case.max_size);
		}
	}
	
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