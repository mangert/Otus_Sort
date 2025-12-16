#include <iostream>
#include "Sorter.h"
#include "Test.h"
#include "test_cases.cpp"
#include "generator.cpp"

int main() {
	/*
	// Получаем все тестовые случаи
	auto test_cases_32 = get_all_tests_uint32();
	auto test_cases_8 = get_all_tests_uint8();

	std::string folders[] = { "0.random", "1.digits", "2.sorted", "3.revers" };

	// Проходим по алгоритмам всем алгоритмам
	for (size_t counter = 0; counter != test_cases_32.size(); ++counter) {
		std::cout << "\n\n=========================================\n";
		std::cout << "ALGORITHM: " << test_cases_32[counter].name << "\n";
		std::cout << "=========================================\n";

		// Проходим по папкам (кроме digits)
		for (const auto& folder : folders) {
			std::cout << "\n--- Data type: " << folder << " ---\n";
			if (folder == "1.digits") { // digits тестируем отдельно
				Test<uint8_t> test(test_cases_8[counter].function, folder);
				test.run_all(test_cases_8[counter].max_size);
			}
			else {
				Test<uint32_t> test(test_cases_32[counter].function, folder);
				test.run_all(test_cases_32[counter].max_size);
			}
		}
	}

	//Отдельная проверка записи отсортированного массива в файл после тестирования
	//будем проверять для алгоритма пирамидальной сортировки для всех видов данных
	std::cout << "Sample test with files output\n";
	std::cout << "\n\n=========================================\n";
	std::cout << "ALGORITHM: " << test_cases_32[1].name << "\n";
	std::cout << "=========================================\n";
	for (const auto& folder : folders) {
		std::cout << "\n--- Data type: " << folder << " ---\n";
		if (folder == "1.digits") { // digits тестируем отдельно
			Test<uint8_t> test(test_cases_8[1].function, folder);
			test.run_all(test_cases_8[1].max_size, true);
		}
		else {
			Test<uint32_t> test(test_cases_32[1].function, folder);
			test.run_all(test_cases_32[1].max_size, true);
		}
	}
	*/
	int arr[10];
	data_generator::fill_random<int>(arr, 10, 20);
	data_generator::print_array(arr, 10);
	std::cout << std::endl;

	Sorter<int>::quick_sort(arr, 10);
	data_generator::print_array(arr, 10);
	return 0; 
}