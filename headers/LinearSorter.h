#pragma once
#include <concepts>
#include <memory>
#include <vector>
#include <utility>

template<std::integral T>//будем использовать только целочисленные типы
class LinearSorter {	

public:
	// ---------- Bucket sort -----------------	
	
	//основная функция - всегда вычисляет min/max
	static void bucket_sort(T* data, size_t size) {
		if (size < 2) return;

		// 1. Находим min/max
		auto [min_val, max_val] =min_max(data, size);
		if (min_val == max_val) return;

		// 2. Автоматически выбираем стратегию
		size_t bucket_count = calculate_bucket_count(size, min_val, max_val);

		// 3. Сортируем
		bucket_sort_impl(data, size, min_val, max_val, bucket_count);
	}

	// с количеством корзин равным количеству элементов
	static void bucket_sort_linear(T* data, size_t size) {
		if (size < 2) return;
		auto [min_val, max_val] = min_max(data, size);
		bucket_sort_impl(data, size, min_val, max_val, size);  // N корзин
	}
	// с количеством корзин равным корню из количества элементов
	static void bucket_sort_sqrt(T* data, size_t size) {
		if (size < 2) return;
		auto [min_val, max_val] = min_max(data, size);
		size_t bucket_count = std::max(size_t(1),
			static_cast<size_t>(std::sqrt(size)));
		bucket_sort_impl(data, size, min_val, max_val, bucket_count);
	}	

private:	
	// ---------- Вспомогательная структура для BucketSort -----------------
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
	// ---------- Общие вспомогательные функции -----------------
	//минимальное и максимальное значение
	static std::pair<T, T> min_max(T* data, size_t size) {
		
		if (size == 0) return { T{}, T{} };
		
		T min = data[0];
		T max = data[0];
		for (size_t i = 1; i != size; ++i) {
			if (data[i] > max) max = data[i];
			if (data[i] < min) min = data[i];
		}
		return { min, max };
	}	

	// ---------- Служебные функции для Bucket sort -----------------
	//общая реализация
	static void bucket_sort_impl(T* data, size_t size, T min_val, T max_val, size_t bucket_count) {

		if (min_val == max_val) return;
		std::vector<std::unique_ptr<Node>> buckets(bucket_count); //наши корзинки
		T offset = 0; //смещение - для обработки отрицательных чисел
		if (min_val < 0) {
			offset = -min_val;
		};
		try {
			for (size_t i = 0; i != size; ++i) {
				size_t bucket = get_bucket_index(data[i], min_val, max_val, offset, bucket_count);
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
	};
	//функция для определения количества корзин
	static size_t calculate_bucket_count(size_t n, T min_val, T max_val) {
		
		if (n < 1000) return n;  // для маленьких массивов

		uint64_t range = static_cast<uint64_t>(max_val) -
			static_cast<uint64_t>(min_val) + 1;

		// Если диапазон маленький
		if (range < 1000 && n > 10000) {
			return static_cast<size_t>(range);
		}

		// По умолчанию sqrt(n)
		return static_cast<size_t>(std::sqrt(n));
	};
	
	//функция для получения индекса "корзины"
	static size_t get_bucket_index(T elem, T min_val, T max_val, T offset, size_t bucket_count) {
		// Все значения неотрицательные после смещения
		uint64_t shifted_elem = static_cast<uint64_t>(elem + offset);
		uint64_t shifted_max = static_cast<uint64_t>(max_val + offset);

		uint64_t normalized = shifted_elem; 
		uint64_t range = shifted_max - (min_val + offset) + 1;

		// Проверка на переполнение при умножении
		if (normalized > std::numeric_limits<uint64_t>::max() / bucket_count) {
			throw std::overflow_error("Bucket index overflow");
		}

		uint64_t index = (normalized * bucket_count) / range;

		if (index >= bucket_count) index = bucket_count - 1;
		return static_cast<size_t>(index);
	}
	
	//функция для добавления элемента в "корзину"
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
