#pragma once
#include <concepts>
#include <functional>
#include <string>
#include <vector>

template<typename T>
requires std::totally_ordered<T> //принимаем только типы, у которых есть оператор < (для простоты - чтобы компаратор не передавать)
class Sorter {

public:        
// ---------- Bubble sort -----------------

    //обычная сортировка пузырьком
    static void bubble_sort(T* data, size_t size) {

        for (size_t i = size - 1; i != 0; --i) {
            for (size_t j = 0; j < i; ++j) {
                if (data[j] > data[j + 1]) {
                    swap_idx(data, j, j + 1);
                }
            }
        }
    };

    //модифицированная сортировка пузырьком
    static void bubble_sort_opt(T* data, size_t size) {        
        for (size_t i = size - 1; i != 0; --i) {
            bool swapped = false;
            for (size_t j = 0; j < i; ++j) {
                if (data[j] > data[j + 1]) {
                    swap_idx(data, j, j + 1);
                    swapped = true;
                }                
            }
            if (!swapped) break;
        }
    };
// ---------- Insertion sort -----------------    
    
    //сортировка вставками
    static void insertion_sort(T* data, size_t size) {

        for (size_t i = 1; i != size; ++i) {
            for (size_t j = i; j > 0 && (data[j - 1] > data[j]); --j) {
                        swap_idx(data, j, j - 1);
             }
        }
    };

    //модифицированная сортировка вставками (со сдвигом)
    static void insertion_sort_shift(T* data, size_t size) {
        
        for (size_t i = 1; i < size; ++i) {
            // Сохраняем текущий элемент
            T curr = data[i];
            
            size_t j = i;
            // Сдвигаем элементы вправо
            while (j > 0 && curr < data[j - 1]) {
                data[j] = data[j - 1];  // сдвиг вместо обмена
                --j;
            };

            // Вставляем рассматриваемый элемент на выбранную позицию на правильное место
            data[j] = curr;
        };
    }
    
    //сортировка вставками с бинанрным поиском
    static void insertion_sort_bin(T* data, size_t size) {
        
        for (size_t i = 1; i < size; ++i) {
            T curr = data[i];

            // Ищем позицию для curr в уже отсортированной части [0..i-1]
            size_t pos = binary_find(data, curr, i); 

            // Сдвигаем элементы вправо от pos до i-1
            for (size_t j = i; j > pos; --j) {
                data[j] = data[j - 1];
            }

            // Вставляем curr на найденную позицию
            data[pos] = curr;
        }
    }
    
    // ---------- Shell sort -----------------    
    
    // 1. Стандартный (gap = n/2, n/4, ...)
    static void shell_sort(T* data, size_t size) {
        if (size <= 1) return;
        shell_sort_core_func(data, size, size / 2,
            [](size_t g) { return g / 2; });
    }

    // 2. Последовательность Кнута (1, 4, 13, 40, 121, ...)
    static void shell_sort_knuth_func(T* data, size_t size) {
        if (size <= 1) return;

        // Вычисляем максимальный gap < size
        size_t gap = 1;
        while (gap < size / 3) {
            gap = 3 * gap + 1;
        }

        shell_sort_core_func(data, size, gap,
            [](size_t g) { return (g - 1) / 3; });  // точная формула
    }

    // 2а Последовательность Кнута с предвычисленным массивом(1, 3, 7, 15, 31, ...)
    static void shell_sort_knuth_arr(T* data, size_t size) {

        if (size > MAX_KNUTH_SIZE) { //длина предрассчитанного массива gap ограничена
            throw std::invalid_argument(
                "shell_sort_knuth: size " + std::to_string(size) +
                " exceeds maximum supported size " + std::to_string(MAX_KNUTH_SIZE)
            );
        };
        if (size <= 1) return;
        shell_sort_core_arr(data, size, KNUTH_GAPS, std::size(KNUTH_GAPS));
    }

    // 3. Последовательность Хиббарда (1, 3, 7, 15, 31, ...)
    static void shell_sort_hibbard_func(T* data, size_t size) {
        if (size <= 1) return;

        // Находим максимальный 2^k - 1 < size
        size_t k = 1;
        while ((1 << k) - 1 < size) ++k;
        size_t gap = (1 << (k - 1)) - 1;  // предыдущая степень

        shell_sort_core_func(data, size, gap,
            [](size_t g) { return (g + 1) / 2 - 1; });
    }
    // 3.а Последовательность Хиббарда с предвычисленным массивом(1, 3, 7, 15, 31, ...)
    static void shell_sort_hibbard_arr(T* data, size_t size) {
        
        if (size > MAX_HIBBARD_SIZE) { //длина предрассчитанного массива gap ограничена
            throw std::invalid_argument(
                "shell_sort_hibbard: size " + std::to_string(size) +
                " exceeds maximum supported size " + std::to_string(MAX_HIBBARD_SIZE)
            );
        };
        if (size <= 1) return;
        shell_sort_core_arr(data, size, HIBBARD_GAPS, std::size(HIBBARD_GAPS));
    }

    // 4. Последовательность Седжвика (1, 5, 19, 41, 109,...)
    static void shell_sort_sedgewick(T* data, size_t size) {
        
        if (size > MAX_SEDGEWICK_SIZE) { //длина предрассчитанного массива gap ограничена
            throw std::invalid_argument(
                "shell_sort_sedgewick: size " + std::to_string(size) +
                " exceeds maximum supported size " + std::to_string(MAX_SEDGEWICK_SIZE)
            );
        };
        if (size <= 1) return;                
        shell_sort_core_arr(data, size, SEDGEWICK_GAPS, std::size(SEDGEWICK_GAPS));    
    }
    // ---------- Selection sort -----------------    
    //сортировка выбором
    static void selection_sort(T* data, size_t size) {

        for (size_t i = 0; i != size - 1; ++i) {
            size_t min_idx = i;
            for (size_t j = i + 1; j != size; ++j) {
                if (data[j] < data[min_idx])
                    min_idx = j;
            }          
            swap_idx(data, min_idx, i);
        };
    };

    // ---------- Heap sort -----------------    
    static void heap_sort(T* data, size_t size) {        
        if (size < 2) return;
        
        //1. построение первоначальной кучи
        for(size_t root = size / 2; root > 0;) {
            heapify(data, --root, size);
        };        
        
        //2. Собственно сортировка
        for (size_t i = size; i > 0;) {            
            swap_idx(data, 0, --i);
            heapify(data, 0, i);
        };        
    };

    // ---------- Quick sort -----------------
    static void quick_sort(T* data, size_t size) {                
        _quick_sort(data, 0, static_cast<int64_t>(size - 1));        
    };    

    // оптимизированная версия (с выбором pivot)
    static void quick_sort_optimized(T* data, size_t size) {
        _quick_sort_optimized(data, 0, static_cast<int64_t>(size - 1));
    };

    // версия с защитой от переполнения стека
    static void quick_sort_safe(T* data, size_t size) {
        if (size < 2) return;
        int depth_limit = 3 * static_cast<int>(log2(size)); //максимально допустимая глубина рекурсии - подбором
        _quick_sort_safe(data, 0, static_cast<int64_t>(size) - 1, depth_limit);
    };

    /*static void quick_sort_safe(T* data, size_t size) {
        heap_sort_fallback_count = 0;
        max_recursion_depth = 0;

        if (size < 2) return;
        int depth_limit = 3 * static_cast<int>(log2(size));

        _quick_sort_safe(data, 0, static_cast<int64_t>(size) - 1, depth_limit);

        // Вывод статистики (только для больших массивов)
        if (size > 10000) {
            std::cout << "  [Stats] Heap sort fallbacks: " << heap_sort_fallback_count
                << ", Max recursion depth: " << max_recursion_depth
                << " (limit was: " << depth_limit << ")\n";
        }
    }*/
    
    // ---------- Merge sort -----------------
    static void merge_sort(T* data, size_t size) {
        
        try {
            // Единый буфер (RAII гарантирует освобождение)
            std::vector<T> temp(size);

            // Рекурсивная сортировка
            _merge_sort(data, temp.data(), 0, size - 1);

        }
        catch (const std::bad_alloc& e) {
            // Не хватило памяти для временного массива
            throw std::runtime_error("Merge sort: not enough memory for temporary array");
        }
    };    
    
private:    
    //================= Константы для версий сортировок Шелла ============//
    //Последовательность Седживика
    static constexpr size_t SEDGEWICK_GAPS[] = {
    1, 5, 19, 41, 109, 209, 505, 929, 2161, 3905, 8929,
    16001, 36289, 64769, 146305, 260609, 587521, 1045505,
    2354689, 4188161, 9427969, 16764929, 37730305, 67084289,
    150958081, 268386305, 603906049, 1073643521, 2415771649
    // для размеров 32 бит
    };
    static constexpr size_t MAX_SEDGEWICK_SIZE = 4831543297ULL; //(2415771649 * 2 - 1);
    
    // Последовательность Кнута
    static constexpr size_t KNUTH_GAPS[] = {
        1, 4, 13, 40, 121, 364, 1093, 3280, 9841, 29524,
        88573, 265720, 797161, 2391484, 7174453, 21523360,
        64570081, 193710244, 581130733, 1743392200
    };

    static constexpr size_t MAX_KNUTH_SIZE = 3486784399ULL; //(1743392200 - 1) * 2 - 1; // 3486784399

    //Последовательность Хиббарда
    static constexpr size_t HIBBARD_GAPS[] = {
        1, 3, 7, 15, 31, 63, 127, 255, 511, 1023,
        2047, 4095, 8191, 16383, 32767, 65535, 131071, 262143,
        524287, 1048575, 2097151, 4194303, 8388607, 16777215,
        33554431, 67108863, 134217727, 268435455, 536870911,
        1073741823, 2147483647
    };

    static constexpr size_t MAX_HIBBARD_SIZE = 4294967295ULL;//(2147483647 - 1) * 2 - 1;

    //================= Вспомогательные обобщенные функции ============//
    // Обобщенная версия сортировки Шелла с функцией получения следующего gap
    template<typename GapFunc>
    static void shell_sort_core_func(T* data, size_t size, size_t initial_gap, GapFunc next_gap) {
        if (size <= 1) return;

        size_t gap = initial_gap;

        while (gap > 0) {
            for (size_t i = gap; i < size; ++i) {
                T current = data[i];
                size_t j = i;

                while (j >= gap && data[j - gap] > current) {
                    data[j] = data[j - gap];
                    j -= gap;
                }

                data[j] = current;
            }

            gap = next_gap(gap);
        }
    }

    // Обобщенная версия сортировки Шелла с предрассчитанным массивом gap
    static void shell_sort_core_arr(T* data, const size_t size, const uint64_t* gaps, const size_t gaps_size) {
        
        size_t initial_idx = binary_find_gap(gaps, size, gaps_size);
        for (size_t i = 0; i <= initial_idx; ++i) {
            size_t gap_idx = initial_idx - i;  // обратный порядок
            size_t gap = gaps[gap_idx];

            for (size_t i = gap; i < size; ++i) {
                T current = data[i];
                size_t j = i;
                while (j >= gap && data[j - gap] > current) {
                    data[j] = data[j - gap];
                    j -= gap;
                }
                data[j] = current;
            }
        }
    };
    
    //================= Служебные вспомогательные функции ============//
    
    //служебная функция обмена элементов с заданными индексами    
    static void swap_idx(T* data, size_t a, size_t b) noexcept {
        T tmp = data[a];
        data[a] = data[b];
        data[b] = tmp;
    };

    //поиск (бинарный) места для вставки
    static size_t binary_find(const T* data, const T& val, size_t n) noexcept {
        // Ищем позицию для вставки val в отсортированный массив data[0..n-1]
        size_t low = 0;
        size_t high = n;  // high = n, потому что вставлять можно после последнего элемента

        while (low < high) {
            size_t mid = low + (high - low) / 2;

            if (val > data[mid]) {
                low = mid + 1;  // Ищем в правой половине
            }
            else {
                high = mid;     // Ищем в левой половине (включая текущий mid)
            }
        }
        return low;  // low == high - позиция для вставки
    };

    //поиск (бинарный) для начального gap для сортировок
    static size_t binary_find_gap(const uint64_t* data, const uint64_t& val, const size_t n) noexcept {
        
        if (n == 0 || data[0] >= val) return 0;  // нет подходящих

        size_t low = 0;    // гарантированно < val
        size_t high = n;   // может быть >= val

        // Инвариант: data[left] < val, data[right] может быть >= val
        while (high - low > 1) {
            size_t mid = low + (high - low) / 2;

            if (data[mid] < val) {
                low = mid;  // mid тоже < val, двигаем левую границу
            }
            else {
                high = mid;  // mid >= val, двигаем правую границу
            }
        }        
        return low;
    }
    
    //рекурсивная функция превращения массива в кучу
    static void heapify(T* data, size_t root, size_t size) {
        size_t max = root;
        size_t left = 2 * root + 1;
        size_t right = 2 * root + 2;
        if (left < size && data[left] > data[max]) max = left;
        if (right < size && data[right] > data[max]) max = right;
        if (max == root) return;
        swap_idx(data, root, max);
        heapify(data, max, size);
    };

    //служебные функции для quick sort
    //функция разделения массива
    static int64_t partition(T* data, int64_t left, int64_t right) {
        T pivot = data[right];
        int64_t m = left - 1;

        for (int64_t j = left; j <= right; ++j) {
            if (data[j] <= pivot) {                
                swap_idx(data, ++m, j);
            }
        }        
        return m;
    }
    
    //служебная функция выбора опорного элемента (для оптимизированной версии quick_sort)
    static int64_t choose_pivot(T* data, int64_t left, int64_t right) {
        int64_t mid = left + (right - left) / 2;

        // Сортируем left, mid, right
        if (data[left] > data[mid]) swap_idx(data, left, mid);
        if (data[left] > data[right]) swap_idx(data, left, right);
        if (data[mid] > data[right]) swap_idx(data, mid, right);

        return mid;  // mid теперь медиана
    }

    //оптимизированная функция разделения для quick_sort (c выбором медианного pivot)
    static int64_t partition_optimized(T* data, int64_t left, int64_t right) {
        // Выбираем медиану и ставим её в конец
        int64_t pivot_idx = choose_pivot(data, left, right);
        swap_idx(data, pivot_idx, right);
        T pivot = data[right];

        int64_t m = left;
        for (int64_t j = left; j < right; ++j) {
            if (data[j] <= pivot) {
                swap_idx(data, m, j);
                ++m;
            }
        }

        swap_idx(data, m, right);
        return m;
    }

    //внутренняя рекурсивная функция быстрой сортировки (обычная)
    static void _quick_sort(T* data, int64_t left, int64_t right) {
        if (left < right) {
            int64_t mid = partition(data, left, right);
            _quick_sort(data, left, mid - 1);
            _quick_sort(data, mid + 1, right);
        }
    }

    //внутренняя рекурсивная функция быстрой сортировки (оптимизированная)
    static void _quick_sort_optimized(T* data, int64_t left, int64_t right) {
        if (left < right) {
            int64_t mid = partition_optimized(data, left, right);
            _quick_sort_optimized(data, left, mid - 1);
            _quick_sort_optimized(data, mid + 1, right);
        }
    }

    //внутренняя рекурсивная функция быстрой сортировки (с защитой от переполненения стека)
    static void _quick_sort_safe(T* data, int64_t left, int64_t right, int depth_limit) {
        
        if (left < 0 || right < 0 || left >= right) return;

        // Если глубина превышена - heap sort
        if (depth_limit == 0) {
            heap_sort(data + left, right - left + 1);
            return;
        };        
        int64_t mid = partition_optimized(data, left, right);
        _quick_sort_safe(data, left, mid - 1, depth_limit - 1);
        _quick_sort_safe(data, mid + 1, right, depth_limit - 1);
    }

    //inline static size_t heap_sort_fallback_count = 0;
    //inline static size_t max_recursion_depth = 0;

    //static void _quick_sort_safe(T* data, int64_t left, int64_t right,
    //    int depth_limit, int current_depth = 0) {

    //    // Следим за глубиной
    //    if (current_depth > max_recursion_depth) {
    //        max_recursion_depth = current_depth;
    //    }

    //    if (left >= right) return;

    //    // Fallback на heap sort
    //    if (depth_limit == 0) {
    //        ++heap_sort_fallback_count;
    //        heap_sort(data + left, right - left + 1);
    //        return;
    //    }

    //    int64_t mid = partition_optimized(data, left, right);

    //    // Балансировка рекурсии
    //    if (mid - left < right - mid) {
    //        _quick_sort_safe(data, left, mid - 1, depth_limit - 1, current_depth + 1);
    //        _quick_sort_safe(data, mid + 1, right, depth_limit - 1, current_depth + 1);
    //    }
    //    else {
    //        _quick_sort_safe(data, mid + 1, right, depth_limit - 1, current_depth + 1);
    //        _quick_sort_safe(data, left, mid - 1, depth_limit - 1, current_depth + 1);
    //    }
    //}

    //служебные функции для merge sort
    //внутренняя рекурсивная функция
    static void _merge_sort(T* data, T* temp, size_t left, size_t right) {

        // Базовый случай
        if (left >= right) return;

        // Вычисляем середину (без переполнения)
        size_t mid = left + (right - left) / 2;

        // Рекурсивно сортируем половины
        _merge_sort(data, temp, left, mid);
        _merge_sort(data, temp, mid + 1, right);

        // Сливаем отсортированные половины
        merge(data, temp, left, mid, right);
    }
    
    //функция объединения отсортированных массивов
    static void merge(T* data, T* temp, size_t left, size_t mid, size_t right) {

        // 1. Размеры частей
        const size_t left_size = mid - left + 1;
        const size_t right_size = right - mid;

        // 2. Копируем во временный массив
        std::copy_n(data + left, left_size, temp + left);
        std::copy_n(data + mid + 1, right_size, temp + mid + 1);


        size_t i = left;      // Левая часть во временном массиве
        size_t j = mid + 1;   // Правая часть во временном массиве
        size_t k = left;      // Текущая позиция в оригинальном массиве

        // Слияние с сохранением стабильности
        while (i <= mid && j <= right) {
            if (temp[i] <= temp[j]) {
                data[k++] = temp[i++];
            }
            else {
                data[k++] = temp[j++];
            }
        }

        // Докопируем остатки
        while (i <= mid) data[k++] = temp[i++];
        while (j <= right) data[k++] = temp[j++];
    }

 };
