#include <iostream>
#include "Sorter.h"
#include "Test.h"
#include "test_cases.cpp"
#include "generator.cpp"
#include "ExternalSorter.cpp"
#include "ExternalTest.cpp"

int main() {
	setlocale(LC_ALL, "Russian");
	//1. тестирование алгоритмов сортировки в памяти
	/*
	{

		// Получаем все тестовые случаи
		auto test_cases_32 = get_all_tests_uint32();
		auto test_cases_8 = get_all_tests_uint8();

		std::string folders[] = { "0.random", "1.digits", "2.sorted", "3.revers" };

		// Проходим по всем алгоритмам
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
					if (folder == "3.revers"
						&& test_cases_32[counter].function == Sorter<uint32_t>::quick_sort)
						test.run_all(9999); //для быстрой сортировки при обратно отсортированном массиве отдельно ограничиваем, а то падает
					else test.run_all(test_cases_32[counter].max_size);
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
		
	}
	*/
	//2. тестирование алгоритмов внешней сортировки
	{

		ExternalTest test; //класс для тестирования

		std::cout << "========= Тестирование алгоритмов внешней сортировки ========\n";
		std::cout << "------ ES1 - Buckets sort\n";
		test.run_all_tests(ExternalSorter::external_sort_method1, 999999, "ES1");

		std::cout << "------ end Buckets sort\n";

		std::cout << "------ ES2 - Balanced two-way merge sort\n";

		auto es2_adapter = [](const std::string& input_file,
			const std::string& output_file,
			[[maybe_unused]] const size_t T, //параметр не используется в этом алгоритме
			[[maybe_unused]] const std::string& temp_dir) {

				ExternalSorter::external_sort_method2(input_file, output_file);
			};
		test.run_all_tests(es2_adapter, 10000000, "ES2");

		std::cout << "------ end merge sort\n";

		std::cout << "------ ES3 - Block sort\n";

		for (size_t i = 1; i <= 2; ++i) {

			size_t block_size = 100 * i;
			std::cout << "\nBlock size = " << block_size << "\n";

			auto es3_adapter = [block_size](const std::string& input_file,
				const std::string& output_file,
				[[maybe_unused]] const size_t T, //параметр не используется в этом алгоритме
				[[maybe_unused]] const std::string& temp_dir) {

					ExternalSorter::external_sort_method3(input_file, output_file,
						block_size, Sorter<uint32_t>::shell_sort_sedgewick);
				};

			test.run_all_tests(es3_adapter, 10000000, "ES3");
		};
	}
	return 0; 
}