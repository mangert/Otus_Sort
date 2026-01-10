#pragma once
#include <iostream>
#include <fstream>
#include <cstdint>
#include <random>
#include <string>
#include <numeric>
#include <chrono>
#include "Sorter.h"

class BinaryFileSorter {

public:       
    // ---------- Counting sort -----------------
    static void counting_sort(std::string& input_file, std::string& output_file,        
        uint16_t max_value = std::numeric_limits<uint16_t>::max()) {

        std::vector<uint64_t> counters(max_value + 1, 0);

        { //в блоке изолируем работу с входным файлом
            std::ifstream in_file(input_file, std::ios::binary);
            if (!in_file) {
                std::cerr << "Ошибка открытия файла: " << input_file << "\n";
                return;
            }

            constexpr size_t BUFFER_SIZE = 1024 * 1024; // 1 МБ чисел
            std::vector<char> byte_buffer(BUFFER_SIZE * 2);            

            while (true) {
                in_file.read(byte_buffer.data(), byte_buffer.size());
                size_t bytes_read = in_file.gcount();
                if (bytes_read == 0) break;

                size_t numbers_read = bytes_read / 2;             

                for (size_t i = 0; i < numbers_read; ++i) {
                    const char* byte_ptr = byte_buffer.data() + i * 2;
                    uint16_t idx = read_uint16_LE(byte_ptr);
                    ++counters[idx];
                }
            }        
        }       
        
        std::ofstream out_file(output_file, std::ios::binary | std::ios::trunc);
        if (!out_file) {
            std::cerr << "Ошибка создания выходного файла: " << output_file << "\n";
            return;
        }

        constexpr size_t WRITE_BUFFER_SIZE = 1024 * 1024; // 1 МБ чисел
        std::vector<char> write_buffer(WRITE_BUFFER_SIZE * 2);
        size_t buffer_pos = 0;
        uint64_t numbers_written = 0;        

        for (size_t value = 0; value <= max_value; ++value) {
            if (counters[value] == 0) continue;
        
            uint64_t count = counters[value];

            while (count > 0) {
                size_t free_space = (write_buffer.size() - buffer_pos) / 2;
                size_t to_write = std::min(static_cast<uint64_t>(free_space), count);

                // Запись в буфер
                for (size_t i = 0; i < to_write; ++i) {
                    // Прямая запись в буфер
                    write_buffer[buffer_pos] = static_cast<char>(value & 0xFF);
                    write_buffer[buffer_pos + 1] = static_cast<char>((value >> 8) & 0xFF);
                    buffer_pos += 2;
                }

                count -= to_write;
                numbers_written += to_write;

                // Сброс буфера в файл
                if (buffer_pos >= write_buffer.size()) {
                    out_file.write(write_buffer.data(), write_buffer.size());
                    buffer_pos = 0;
                }
            }
        }

        // Запись остатка буфера
        if (buffer_pos > 0) {
            out_file.write(write_buffer.data(), buffer_pos);
        }

        out_file.close();
        
    }
    
    // ---------- Radix sort -----------------
    static void radix_sort(const std::string& input, const std::string& output,
        uint16_t base = 256, uint16_t max_value = std::numeric_limits<uint16_t>::max()) {
        
        // Проверяем существование входного файла
        if (!std::filesystem::exists(input)) {
            std::cerr << "Ошибка: входной файл не существует: " << input << std::endl;
            return;
        }        
        
        // Вычисляем количество разрядов
        size_t num_digits = 0;
        if (base > 0) {
            uint32_t temp = max_value;
            do {
                temp /= base;
                num_digits++;
            } while (temp > 0);
        }
        else {
            num_digits = 1;
        }

        std::string current_file = input;
        std::string next_file = output + ".tmp.0";

        // Для каждого разряда (от младшего к старшему)
        for (size_t digit = 0; digit < num_digits; ++digit) {             
            
            counting_sort_by_digit(current_file, next_file, base, digit);

            // Меняем местами имена файлов
            std::swap(current_file, next_file);

            // Сбрасываем next_file для следующей итерации
            next_file = output + ".tmp." + std::to_string(digit + 1);
        }

        std::filesystem::remove(output);
        try {
            std::filesystem::rename(current_file, output);
        }
        catch (...) {
            std::ifstream src(current_file, std::ios::binary);
            std::ofstream dst(output, std::ios::binary);
            dst << src.rdbuf();
            src.close();
            dst.close();
            std::filesystem::remove(current_file);
        }

        // Удаляем все временные файлы
        for (size_t i = 0; i <= num_digits; ++i) {
            std::string tmp_file = output + ".tmp." + std::to_string(i);
            if (std::filesystem::exists(tmp_file)) {
                std::filesystem::remove(tmp_file);
            }
        }
    }

    // ---------- Bucket sort -----------------
    static void bucket_sort(std::string& input_file, std::string& output_file,
        uint16_t max_value = std::numeric_limits<uint16_t>::max(),
        size_t bucket_count = 256) {

        auto start_time = std::chrono::high_resolution_clock::now();

        // 1. Не открываем все файлы сразу - только имена
        std::vector<std::string> filenames(bucket_count);
        for (size_t i = 0; i < bucket_count; ++i) {
            filenames[i] = output_file + "_b" + std::to_string(i) + ".tmp";
        }

        // 2. Распределение с открытием файлов по мере необходимости
        {
            std::ifstream in_file(input_file, std::ios::binary);
            if (!in_file) {
                std::cerr << "Ошибка открытия входного файла" << std::endl;
                return;
            }

            constexpr size_t BUFFER_SIZE = 1024 * 1024;
            std::vector<char> byte_buffer(BUFFER_SIZE * 2);

            // Вектор для отслеживания открытых файлов
            std::vector<std::unique_ptr<std::ofstream>> bucket_files(bucket_count);

            uint64_t numbers_processed = 0;
            constexpr size_t FLUSH_INTERVAL = 10000; // Закрываем файлы периодически

            while (true) {
                in_file.read(byte_buffer.data(), byte_buffer.size());
                size_t bytes_read = in_file.gcount();
                if (bytes_read == 0) break;

                size_t numbers_read = bytes_read / 2;

                for (size_t i = 0; i < numbers_read; ++i) {
                    const char* byte_ptr = byte_buffer.data() + i * 2;
                    uint16_t number = read_uint16_LE(byte_ptr);
                    size_t bucket_idx = get_bucket_index(number, max_value, bucket_count);

                    // Открываем файл если нужно
                    if (!bucket_files[bucket_idx]) {
                        bucket_files[bucket_idx] = std::make_unique<std::ofstream>(
                            filenames[bucket_idx],
                            std::ios::binary | std::ios::app
                        );
                    }

                    // Используем нашу функцию записи вместо reinterpret_cast
                    write_uint16_LE(*bucket_files[bucket_idx], number);
                }

                numbers_processed += numbers_read;

                // Периодически закрываем файлы чтобы освободить дескрипторы
                if (numbers_processed % FLUSH_INTERVAL == 0) {
                    for (auto& file_ptr : bucket_files) {
                        if (file_ptr) {
                            file_ptr->flush();
                            // Можно закрыть, но тогда придется открывать снова
                            // file_ptr.reset();
                        }
                    }
                }
            }

            // Явно закрываем все файлы (деструкторы сделают это, но явно лучше)
            for (auto& file_ptr : bucket_files) {
                if (file_ptr) {
                    file_ptr->close();
                }
            }
        }

        // 3. Сортировка и объединение
        std::cout << "  Сортировка корзин..." << std::endl;
        std::ofstream out_file(output_file, std::ios::binary | std::ios::trunc);
        if (!out_file) {
            std::cerr << "Ошибка создания выходного файла" << std::endl;
            return;
        }

        size_t total_written = 0;

        for (size_t i = 0; i < bucket_count; ++i) {
            const std::string& bucket_file = filenames[i];

            if (!std::filesystem::exists(bucket_file)) continue;

            uint64_t bucket_size = std::filesystem::file_size(bucket_file);
            if (bucket_size == 0) {
                std::filesystem::remove(bucket_file);
                continue;
            }

            uint64_t bucket_numbers = bucket_size / sizeof(uint16_t);

            // Читаем корзину
            std::vector<uint16_t> bucket_data(bucket_numbers);
            {
                std::ifstream b_file(bucket_file, std::ios::binary);
                if (!b_file) {
                    std::cerr << "Ошибка открытия корзины " << i << std::endl;
                    continue;
                }

                // Читаем через буфер
                const size_t READ_BUF_SIZE = 1024 * 1024;
                std::vector<char> read_buffer(READ_BUF_SIZE * 2);
                size_t total_read = 0;

                while (total_read < bucket_size) {
                    size_t to_read = std::min(READ_BUF_SIZE * 2, bucket_size - total_read);
                    b_file.read(read_buffer.data(), to_read);
                    size_t bytes_read = b_file.gcount();

                    // Копируем в bucket_data
                    size_t numbers_in_buf = bytes_read / sizeof(uint16_t);
                    for (size_t j = 0; j < numbers_in_buf; ++j) {
                        const char* byte_ptr = read_buffer.data() + j * sizeof(uint16_t);
                        bucket_data[total_read / sizeof(uint16_t) + j] = read_uint16_LE(byte_ptr);
                    }

                    total_read += bytes_read;
                }
            }

            // Сортируем
            std::sort(bucket_data.begin(), bucket_data.end());

            // Записываем чистым способом
            for (uint16_t num : bucket_data) {
                write_uint16_LE(out_file, num);
            }

            total_written += bucket_numbers;
            std::filesystem::remove(bucket_file);

            if ((i + 1) % 100 == 0) {
                std::cout << "  Обработано " << (i + 1) << "/" << bucket_count
                    << " корзин" << std::endl;
            }
        }

        out_file.close();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);

        std::cout << "BucketSort завершен за " << duration.count() / 1000.0
            << " секунд" << std::endl;
        std::cout << "  Записано чисел: " << total_written << std::endl;
    }
    
    // ---------- Генерация бинарного файла со случайными 16-битными числами
    // Формат файла: little-endian (младший байт первый)
    static void generate_random_file(uint64_t n, std::string& filename) {

        std::cout << "Генерация файла с " << n << " числами ("
            << (n * 2) / (1024.0 * 1024.0) << " МБ)...\n";


        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file) {
            std::cerr << "Ошибка открытия файла\n";
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> distrib(0, std::numeric_limits<uint16_t>::max());

        const size_t buffer_size = 1024 * 1024;
        std::vector<uint16_t> buffer(buffer_size);

        uint64_t numbers_written = 0;

        while (numbers_written < n) {
            size_t block_size = static_cast<size_t>(
                std::min<uint64_t>(buffer_size, n - numbers_written)
                );

            for (size_t i = 0; i < block_size; ++i) {
                buffer[i] = distrib(gen);
            }

            // Сериализация каждого числа
            for (size_t i = 0; i < block_size; ++i) {
                // Разбиваем 16-битное число на 2 байта и записываем в файл
                write_uint16_LE(file, buffer[i]);                
            }

            if (!file) {
                std::cerr << "Ошибка записи!\n";
                return;
            }

            numbers_written += block_size;
        }
    };

    // Минимальная версия для быстрой проверки - убрать!!!
    static void peekBinaryFile(const std::string& filename, int n = 20) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Не могу открыть " << filename << std::endl;
            return;
        }
        std::cout << filename << ": ";

        uint16_t num;
        for (int i = 0; i < n && file.read(reinterpret_cast<char*>(&num), 2); i++) {
            std::cout << num << " ";
        }
        std::cout << "..." << std::endl;
        file.close();
    }

private:
    //хелперы для чтения-записи
    //собираем байты в число
    static inline uint16_t read_uint16_LE(const char* bytes) {
        // Little Endian: младший байт первый
        return static_cast<uint16_t>(
            static_cast<uint8_t>(bytes[0]) |
            (static_cast<uint8_t>(bytes[1]) << 8)
            );
    }

    //записываем число в буфер как 2 байта
    static inline void write_uint16_LE(std::ostream& stream, uint16_t value) {
        char bytes[2];
        bytes[0] = static_cast<char>(value & 0xFF);
        bytes[1] = static_cast<char>((value >> 8) & 0xFF);
        stream.write(bytes, 2);
    }

    //сериализуем число в байты
    static inline void uint16_to_bytes_LE(char* bytes, uint16_t value) {
        // Little Endian: младший байт первый
        bytes[0] = static_cast<char>(value & 0xFF);
        bytes[1] = static_cast<char>((value >> 8) & 0xFF);
    }

    //----------вспомогательные функции для Radix Sort -----
    //Получение digit-го разряда числа в системе base
    static inline uint8_t get_digit(uint16_t number, int digit, uint16_t base) {
        if (base == 0) return 0;
        uint32_t divisor = 1;
        for (size_t i = 0; i < digit; ++i) {
            divisor *= base;
            if (divisor == 0) return 0; // Защита от переполнения
        }
        return (number / divisor) % base;
    }

    // Стабильная сортировка подсчетом по одному разряду
    static void counting_sort_by_digit(const std::string& input, const std::string& output,
        uint16_t base, int digit) {

        // 1. Определяем размер файла
        std::ifstream in_file(input, std::ios::binary | std::ios::ate);
        if (!in_file) {
            std::cerr << "Ошибка открытия файла: " << input << std::endl;
            return;
        }

        uint64_t file_size = in_file.tellg();
        uint64_t total_numbers = file_size / sizeof(uint16_t);
        in_file.close();

        if (total_numbers == 0) {
            // Пустой файл
            std::ofstream out_file(output, std::ios::binary);
            out_file.close();
            return;
        }

        // 2. Подсчет частот для текущего разряда
        std::vector<uint64_t> counters(base, 0);

        in_file.open(input, std::ios::binary);
        if (!in_file) {
            std::cerr << "Ошибка открытия файла: " << input << std::endl;
            return;
        }

        // Буферизованное чтение для подсчета
        constexpr size_t BUFFER_SIZE = 1024 * 1024; // 1 МБ чисел
        constexpr size_t BYTE_BUFFER_SIZE = BUFFER_SIZE * sizeof(uint16_t);
        std::vector<char> byte_buffer(BYTE_BUFFER_SIZE);

        while (true) {
            in_file.read(byte_buffer.data(), byte_buffer.size());
            size_t bytes_read = in_file.gcount();

            if (bytes_read == 0) break;

            size_t numbers_read = bytes_read / sizeof(uint16_t);

            for (size_t i = 0; i < numbers_read; ++i) {
                const char* byte_ptr = byte_buffer.data() + i * sizeof(uint16_t);
                uint16_t num = read_uint16_LE(byte_ptr);
                uint8_t digit_value = get_digit(num, digit, base);
                ++counters[digit_value];
            }
        }
        in_file.close();

        // 3. Префиксные суммы        
        std::partial_sum(counters.begin(), counters.end(), counters.begin());
        
        // 4. Второй проход: распределение чисел
        in_file.open(input, std::ios::binary);
        std::ofstream out_file(output, std::ios::binary | std::ios::trunc);

        if (!out_file) {
            std::cerr << "Ошибка создания файла: " << output << std::endl;
            return;
        }
        // Создаем выходной файл нужного размера
        out_file.seekp(file_size - 1);
        out_file.write("", 1);
        out_file.close();

        std::fstream out_file_rw(output, std::ios::binary | std::ios::in | std::ios::out);

        // Читаем файл буферами c конца
        std::ifstream in_file_second_pass(input, std::ios::binary);
        if (!in_file_second_pass) {
            std::cerr << "Ошибка открытия файла для второго прохода: " << input << std::endl;
            return;
        }

        // Устанавливаем в конец и сбрасываем флаги
        in_file_second_pass.seekg(0, std::ios::end);
        in_file_second_pass.clear();  //сбрасываем флаги eof/fail

        uint64_t remaining_numbers = total_numbers;        
        constexpr size_t BUFFER_BYTES = 1024 * 1024 * sizeof(uint16_t); // 2 МБ
        std::vector<char> _byte_buffer(BUFFER_BYTES);

        // Проверяем позицию в файле
        std::streampos pos = in_file_second_pass.tellg();
        
        while (remaining_numbers > 0) {
            // Сколько байт читать
            size_t bytes_to_read = std::min(
                BUFFER_BYTES,
                remaining_numbers * sizeof(uint16_t)
            );
            size_t elements_to_read = bytes_to_read / sizeof(uint16_t);

            // Позиция в файле
            uint64_t block_start_byte = (remaining_numbers - elements_to_read) * sizeof(uint16_t);
            
            in_file_second_pass.seekg(block_start_byte);  
            in_file_second_pass.clear(); // cбрасываем флаги

            // Читаем байты из потока
            in_file_second_pass.read(_byte_buffer.data(), bytes_to_read);
            size_t bytes_read = in_file_second_pass.gcount();  
            size_t elements_read = bytes_read / sizeof(uint16_t);

            if (elements_read == 0) break;

            // Обрабатываем в обратном порядке
            for (int64_t i = elements_read - 1; i >= 0; --i) {
                const char* byte_ptr = _byte_buffer.data() + i * sizeof(uint16_t);
                uint16_t num = read_uint16_LE(byte_ptr);
                uint8_t digit_value = get_digit(num, digit, base);

                uint64_t pos = --counters[digit_value];
                uint64_t byte_pos = pos * sizeof(uint16_t);

                out_file_rw.seekp(byte_pos);
                write_uint16_LE(out_file_rw, num);
            }
            remaining_numbers -= elements_read;
        }

        in_file_second_pass.close();
        out_file_rw.close();
    }

    //---------вспомогательные функции для Bucket Sort -------
    static size_t get_bucket_index(uint16_t number, uint16_t max_value, size_t bucket_count) {

        uint64_t index = (static_cast<uint64_t>(number) * static_cast<uint64_t>(bucket_count))
            / (static_cast<uint64_t>(max_value) + 1);

        return static_cast<size_t>(index);
    };

    // Структура для хранения буфера корзины
    struct BucketBuffer {
        std::vector<uint16_t> buffer;
        std::string filename;
        size_t count = 0;

        BucketBuffer(const std::string& base_name, size_t bucket_id)
            : filename(base_name + "_bucket_" + std::to_string(bucket_id) + ".tmp") {
            buffer.reserve(1024 * 1024); // 1 МБ чисел
        }

        ~BucketBuffer() {
            flush();
        }

        void add(uint16_t value) {
            buffer.push_back(value);
            count++;
            if (buffer.size() >= buffer.capacity()) {
                flush();
            }
        }

        void flush() {
            if (buffer.empty()) return;

            std::ofstream file(filename, std::ios::binary | std::ios::app);
            if (file) {
                // Записываем все числа разом
                file.write(reinterpret_cast<const char*>(buffer.data()),
                    buffer.size() * sizeof(uint16_t));
            }
            buffer.clear();
        }
    };

    // Функция для добавления в корзину
    void add_to_bucket(uint16_t number, size_t bucket_index,
        std::vector<BucketBuffer>& buckets) {
        if (bucket_index >= buckets.size()) {
            std::cerr << "Ошибка: индекс корзины " << bucket_index
                << " вне диапазона!" << std::endl;
            return;
        }
        buckets[bucket_index].add(number);
    }
};
