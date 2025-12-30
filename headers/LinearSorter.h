#pragma once
#include <concepts>
#include <memory>
#include <vector>
#include <utility>

template<std::integral T>//будем использовать только целочисленные типы
class LinearSorter {	

public:
	// ---------- Bucket sort -----------------
	static void bucket_sort(T* data, size_t size, T min_val, T max_val) {		

		if ((size < 2) || min_val == max_val) return;

		//size_t bucket_count = static_cast<size_t>(std::sqrt(size)); //в другой версии сделаю
		size_t bucket_count = size;
		
		std::vector<std::unique_ptr<Node>> buckets(bucket_count); //наши корзинки
		
		try {
			for (size_t i = 0; i != size; ++i) {
				size_t bucket = get_bucket_index(data[i], min_val, max_val, bucket_count);
				add_to_bucket(buckets[bucket], data[i]);
			}

			size_t idx = 0;
			for (auto& bucket_head : buckets) {
				Node* current = bucket_head.get();
				while (current != nullptr) {
					data[idx++] = current->key;
					current = current->next.get();
				}
			}
		}
		catch (std::exception& e) {
			throw e;
		}
	}

private:	
	struct Node {
		T key;
		std::unique_ptr<Node> next;
	
		explicit Node(T key) : key(key), next(nullptr) {};
		Node(const Node&) = delete;
		Node(Node&&) = delete;		
		Node& operator=(Node&&) = delete;
		Node& operator=(const Node&) = delete;
		
		~Node() = default;
	};

	template<typename U, typename V>
	static auto safe_multiply(U a, V b) -> decltype(a * b) {
		using ResultType = decltype(a * b);

		// Проверяем, не будет ли переполнения
		if (a > 0 && b > 0) {
			if (a > std::numeric_limits<ResultType>::max() / b) {
				// Переполнение! Используем double
				throw std::overflow_error("Переполнение при умножении");
			}
		}
		return a * b;
	}

	static size_t get_bucket_index(T elem, T min_val, T max_val, size_t bucket_count) {
		
		// Нормализуем элемент в [0, 1)
		// Сначала вычитаем min, чтобы работать с неотрицательными числами
		uint64_t normalized = elem - min_val;

		// 3. Вычисляем диапазон
		uint64_t range;
		if (max_val > min_val) {
			range = (max_val - min_val) + 1;
		}
		else {
			range = 1;  // все элементы одинаковые
		}

		// 4. Вычисляем индекс
		size_t index;
		try {
			uint64_t product = safe_multiply(normalized, static_cast<uint64_t>(bucket_count));
			index = product / range;
		}
		catch (std::exception& e) {
			throw e;
		}
		return index;
	}

	static void add_to_bucket(std::unique_ptr<Node>& head, T key) {
		// Создаём новый узел
		auto new_node = std::make_unique<Node>(key);

		// 1. Вставка в начало
		if (!head || key < head->key) {
			new_node->next = std::move(head);
			head = std::move(new_node);
			return;
		}

		// 2. Ищем место для вставки
		Node* current = head.get();  // Сырой указатель для обхода

		while (current->next && current->next->key < key) {
			current = current->next.get();
		}

		// 3. Вставляем после current
		new_node->next = std::move(current->next);
		current->next = std::move(new_node);
	};
};
