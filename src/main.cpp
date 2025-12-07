#include <iostream>
#include "generator.cpp"
#include "Sorter.h"
#include "Test.h"

int main() {
	
	/*Test<uint32_t> test([](uint32_t* data, size_t size) {
		Sorter<uint32_t>::bubble_sort(data, size);
		}, "1.digits");*/
	Test<uint32_t> test(Sorter<uint32_t>::shell_sort_sedgewick, "1.digits");
	test.run_all();
	uint64_t rnd_arr[10];
	data_generator::fill_random<uint64_t>(rnd_arr, 10, 20);
	data_generator::print_array<uint64_t>(rnd_arr, 10);

	std::cout << std::endl;	
	Sorter<uint64_t>::shell_sort_sedgewick(rnd_arr, 10);

	data_generator::print_array<uint64_t>(rnd_arr, 10);



	return 0; 
}