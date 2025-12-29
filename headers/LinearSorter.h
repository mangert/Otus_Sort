#pragma once
#include <concepts>


template<std::integral T>//будем использовать только целочисленные типы
class LinearSorter {	

public:
	// ---------- Bucket sort -----------------
	static void bucket_sort(T* data, size_t size, T min_val, T max_val) {

		

		if ((size < 2) || min_val == max_val) return;

		size_t bucket_count = static_cast<size_t>(std::sqrt(size));
		std::vector<inner_list*> buckets;

		auto get_bucket = [&size, &max_val](T element) {
			return static_cast<size_t>((element * size) / (max_val + 1)) //переполнение? Разделить? И что если отрицательное?
			};
		for (size_t i = 0; i != size; ++i) {

			size_t bucket = get_bucket(data[i]);
			

		}
		
		


	}
private:
	
	struct Node { //тут подумать надо... не доделано
		T key;
		Node* next;
	
		Node(T key, Node* next) key(key) next(next) {};
		Node(const Node&) = delete;
		Node(Node&&) = delete;		
		Node& operator=(Node&&) = delete;
		Node& operator=(const Node&) = delete;
		~Node() = default;
	};



};
