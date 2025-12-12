#include <iostream>
#include "Sorter.h"
#include "Test.h"
#include "test_cases.cpp"
#include "generator.cpp"

int main() {
	
	// Получаем все тестовые случаи
	auto test_cases_32 = get_all_tests_uint32();
	auto test_cases_8 = get_all_tests_uint8();

	std::string folders[] = { "0.random", "1.digits", "2.sorted", "3.revers" };

	// Проходим по алгоритмам всем алгоритмам
	/*for (size_t counter = 0; counter != test_cases_32.size(); ++counter) {
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
	//будем проверять для алгоритма последовательности Седжвика всех видов данных
	std::cout << "Sample test with files output\n";
	std::cout << "\n\n=========================================\n";
	std::cout << "ALGORITHM: " << test_cases_32[11].name << "\n";
	std::cout << "=========================================\n";
	for (const auto& folder : folders) {
		std::cout << "\n--- Data type: " << folder << " ---\n";
		if (folder == "1.digits") { // digits тестируем отдельно
			Test<uint8_t> test(test_cases_8[11].function, folder);
			test.run_all(test_cases_8[11].max_size, true);
		}
		else {
			Test<uint32_t> test(test_cases_32[11].function, folder);
			test.run_all(test_cases_32[11].max_size, true);
		}
	}*/
	/*struct TestCase32 {
		using FuncPtr = void(*)(uint32_t*, size_t);
		FuncPtr function;
		std::string name;
		size_t max_size;  // 0 = без ограничений

		TestCase32(FuncPtr func, std::string n, size_t max = 0)
			: function(func), name(std::move(n)), max_size(max) {
		}
	};

	struct TestCase8 {
		using FuncPtr = void(*)(uint8_t*, size_t);
		FuncPtr function;
		std::string name;
		size_t max_size;  // 0 = без ограничений

		TestCase8(FuncPtr func, std::string n, size_t max = 0)
			: function(func), name(std::move(n)), max_size(max) {
		}
	};*/

	
	TestCase32 s1 = { Sorter<uint32_t>::heap_sort, "heap_sort", 0 };
	TestCase8 s2 = { Sorter<uint8_t>::heap_sort, "heap_sort", 0 };



	std::cout << "Sample test with files output\n";
	std::cout << "\n\n=========================================\n";
	std::cout << "ALGORITHM: " << s1.name << "\n";
	std::cout << "=========================================\n";
	for (const auto& folder : folders) {
		std::cout << "\n--- Data type: " << folder << " ---\n";
		if (folder == "1.digits") { // digits тестируем отдельно
			Test<uint8_t> test(s2.function, folder);
			test.run_all(s2.max_size);
		}
		else {
			Test<uint32_t> test(s1.function, folder);
			test.run_all(s1.max_size);
		}
	}	
	
	return 0; 
}