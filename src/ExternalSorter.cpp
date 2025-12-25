#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <random>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <exception>
#include <functional>

//класс содержит статические функции внешних сортировок и функцию, генерирующую файл из случайных чисел
class ExternalSorter {
    
public:

    // ES1: Создание T файлов с последующим слиянием (Bucket Sort)
    static void external_sort_method1(const std::string& input_file,
        const std::string& output_file,
        const size_t T,
        const std::string& temp_dir = "temp_buckets") {

        // 0. Создаём директорию для временных файлов
        create_temp_directory(temp_dir);

        try {
            // 1. Распределяем числа по bucket-файлам
            distribute_to_buckets(input_file, temp_dir, T);

            // 2. Объединяем bucket-файлы в выходной файл
            merge_buckets(output_file, temp_dir, T);

            // 3. Очищаем временную директорию
            cleanup_temp_directory(temp_dir);

        }
        catch (const std::exception& e) {
            // В случае ошибки пытаемся очистить временные файлы
            try {
                cleanup_temp_directory(temp_dir);
            }
            catch (...) {
                // Игнорируем ошибки при очистке
            }
            throw;  // Пробрасываем оригинальную ошибку
        }
    }
          
    // ES2: Сортировка с использованием двух вспомогательных файлов (Balanced two-way merge sort)
    static void external_sort_method2(const std::string& input_file,
        const std::string& output_file, const std::string& temp_dir = "temp_merge") {

        _external_sort_generic(input_file, output_file, temp_dir, distribute_alternately, 1);
    }

    // ES3: Сортировка со считыванием чисел блоками (Block sort)
    static void external_sort_method3(const std::string& input_file,
        const std::string& output_file, 
        const size_t block_size, std::function<void(uint32_t*, size_t)> sort_func, 
        const std::string& temp_dir = "temp_block") {

        _external_sort_generic(input_file, output_file, temp_dir, 
            [&block_size, &sort_func](const auto& input, const auto& out1, const auto& out2) {
                distribute_sorted_blocks(input, out1, out2, block_size, sort_func);
            }, block_size);
    }
    
    //функция записи случайных чисел в файл
    static void generate_file(const std::string& filename,
		size_t n, uint32_t lim) {

        if (lim == 0) {
            std::cerr << "ERROR: lim must be > 0\n";
            return;
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "  WARNING: Cannot open result file: " << filename << "\n";
            return;
        }

        std::random_device rd;
        std::mt19937 gen(rd());  
        std::uniform_int_distribution<uint32_t> distrib(1, lim);

        // Записываем случайные числа
        for (size_t i = 0; i != n; ++i) {
            file << distrib(gen);
            if (i != n - 1) {
                file << '\n';
            }
        }

        if (!file) {
            std::cerr << "ERROR: Write failed for file: " << filename << "\n";
        }
	}
private:
    // Общие вспомогательные функции
    // Создание временной директории
    static void create_temp_directory(const std::string& temp_dir) {
        namespace fs = std::filesystem;
        if (fs::exists(temp_dir)) {
            // Очищаем, если уже существует
            fs::remove_all(temp_dir);
        }

        if (!fs::create_directory(temp_dir)) {
            throw std::runtime_error("Cannot create temp directory: " + temp_dir);
        }      
    }
    
    // Очистка временной директории
    static void cleanup_temp_directory(const std::string& temp_dir) {
        namespace fs = std::filesystem;
        if (fs::exists(temp_dir)) {
            try {
                fs::remove_all(temp_dir);
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Warning: Failed to cleanup temp directory: "
                    << e.what() << "\n";
            }
        }
    }

    //преобразуем данные строки в число
    static uint32_t get_key(const std::string& line) {
        try {
            return std::stoul(line);
        }
        catch (...) {
            throw std::runtime_error("Invalid number in line: " + line);
        }
    }
    //-------- Служебные функции для сортировки ES1 -------------//
    
    // Распределение чисел по bucket-файлам
    static void distribute_to_buckets(const std::string& input_file,
        const std::string& temp_dir, const size_t T) {

        std::ifstream input(input_file);
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open input file: " + input_file);
        }        

        // Буферы для часто встречающихся ключей (оптимизация)
        const size_t BUFFER_LIMIT = 10000;  // Число строк в буфере
        std::unordered_map<size_t, std::string> buffers;
        size_t total_numbers = 0;

        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) continue;

            // Преобразуем строку в число
            size_t key = get_key(line);            

            // Проверяем диапазон
            if (key < 1 || key > T) {
                throw std::runtime_error("Number " + std::to_string(key) +
                    " out of range [1, " + std::to_string(T) + "]");
            }

            // Добавляем в буфер для этого ключа
            buffers[key].append(line).append("\n");
            total_numbers++;

            // Если буфер заполнен - сбрасываем в файл
            if (buffers[key].size() > BUFFER_LIMIT * 10) {  // Примерный расчёт
                append_to_bucket_file(temp_dir, key, buffers[key]);
                buffers[key].clear();
            }
        }

        // Сбрасываем остатки буферов в файлы
        for (auto& [key, buffer] : buffers) {
            if (!buffer.empty()) {
                append_to_bucket_file(temp_dir, key, buffer);
            }
        }        
        input.close();
    }
    
    // Добавление данных в bucket-файл
    static void append_to_bucket_file(const std::string& temp_dir,
        size_t bucket_key, const std::string& data) {

        std::string filename = get_bucket_filename(temp_dir, bucket_key);
        std::ofstream file(filename, std::ios::app);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot open bucket file: " + filename);
        }

        file << data;

        if (!file) {
            throw std::runtime_error("Write failed to bucket file: " + filename);
        }
    }

    // Получение имени bucket-файла
    static std::string get_bucket_filename(const std::string& temp_dir,
        const size_t bucket_key) {
        return temp_dir + "/bucket_" + std::to_string(bucket_key) + ".txt";
    }

    // Объединение bucket-файлов в выходной файл
    static void merge_buckets(const std::string& output_file,
        const std::string& temp_dir, const size_t T) {

        std::ofstream output(output_file);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot open output file: " + output_file);
        }        

        size_t total_written = 0;
        namespace fs = std::filesystem;
        for (size_t key = 1; key <= T; ++key) {
            std::string filename = get_bucket_filename(temp_dir, key);

            if (fs::exists(filename)) {
                std::ifstream bucket_file(filename);
                if (!bucket_file.is_open()) {
                    std::cerr << "Warning: Cannot open bucket file: " << filename << "\n";
                    continue;
                }

                std::string line;
                while (std::getline(bucket_file, line)) {
                    if (!line.empty()) {
                        output << line << "\n";
                        total_written++;

                        // Периодически сбрасываем буфер
                        if (total_written % 100000 == 0) {
                            output.flush();
                        }
                    }
                }

                bucket_file.close();
            }
            // Если файла нет (нет чисел с таким key) - просто пропускаем
        }

        output.close();        
    }
    
    // --------внутренняя функция сортировки ES2, ES3 ------------ -//
    static void _external_sort_generic(const std::string& input_file,
        const std::string& output_file, const std::string& temp_dir,
        std::function<void(std::string, std::string, std::string)> distributor, size_t run_length) {

        // Имена временных файлов
        create_temp_directory(temp_dir);
        const std::string fileA = temp_dir + "/temp_merge_A.txt";
        const std::string fileB = temp_dir + "/temp_merge_B.txt";
        const std::string fileC = temp_dir + "/temp_merge_C.txt";
        const std::string fileD = temp_dir + "/temp_merge_D.txt";

        try {
            // 1. Фаза распределения (distribution phase)            
            distributor(input_file, fileA, fileB);
            
            size_t total_records = count_records(input_file);            
            size_t countA = count_records(fileA);
            size_t countB = count_records(fileB);
            size_t total = countA + countB;            

            std::string source1 = fileA;
            std::string source2 = fileB;
            std::string dest1 = fileC;
            std::string dest2 = fileD;

            // 2. Фаза слияния (merge phase)
            while (run_length < total_records) {                
                // Сливаем серии
                merge_runs_balanced(source1, source2, dest1, dest2, run_length);

                size_t countD1 = count_records(dest1);
                size_t countD2 = count_records(dest2);
                
                if (countD1 + countD2 != total) { //проверка, что ничего не потеряли
                    std::cerr << "ERROR: Lost " << (total - countD1 - countD2)
                        << " records!\n";                    
                }

                // Удваиваем длину серии для следующей итерации
                run_length *= 2;

                // Меняем роли файлов
                std::swap(source1, dest1);
                std::swap(source2, dest2);
            }

            // 3. Определяем, в каком файле результат
            std::string result_file = (count_records(source1) > 0) ? source1 : source2;

            // 4. Копируем результат в выходной файл
            if (result_file != output_file) {
                std::filesystem::copy(result_file, output_file,
                    std::filesystem::copy_options::overwrite_existing);
            }

            // 5. Очистка
            cleanup_temp_directory(temp_dir);

        }
        catch (const std::exception& e) {
            cleanup_temp_directory(temp_dir);
            throw;
        }
    }
    
    //-------- Служебные функции для сортировки ES2, ES3-------------//
    
    // Функция распределения по 2 файлам (ES2)
    static void distribute_alternately(const std::string& input_file,
        const std::string& output1, const std::string& output2) {

        std::ifstream input(input_file);
        std::ofstream out1(output1);
        std::ofstream out2(output2);

        if (!input.is_open() || !out1.is_open() || !out2.is_open()) {
            throw std::runtime_error("Cannot open files for distribution");
        }

        std::string line; //данные
        bool to_first = true; //переключатель  

        while (std::getline(input, line)) {
            if (line.empty()) continue;

            // Проверяем, что это число
            try {
                std::stoull(line);
            }
            catch (...) {
                throw std::runtime_error("Invalid number: " + line);
            }

            if (to_first) {
                out1 << line << '\n';         
            }
            else {
                out2 << line << '\n';              
            }

            to_first = !to_first;
        }        
    }
    
    // Функция распределения по 2 файлам c сортировкой блока (ES3)    
    static void distribute_sorted_blocks(const std::string& input_file,
        const std::string& output1, const std::string& output2,
        const size_t block_size, std::function<void(uint32_t*, size_t)> sort_func) {
        
        std::ifstream input(input_file);
        std::ofstream out1(output1);
        std::ofstream out2(output2);

        if (!input.is_open() || !out1.is_open() || !out2.is_open()) {
            throw std::runtime_error("Cannot open files for distribution");
        }        
        
        std::string line;        
        size_t count1 = 0, count2 = 0;
        
        std::vector<uint32_t> block;
        block.reserve(block_size);

        auto save_to_file = [&block](std::ofstream* out) { //мини-функция записи числа из вектора в файл
            for(auto& it : block)             {
                *out << it << '\n';
            }
        };
        std::ofstream* current_output = &out1;

        while (read_next_valid(input, line)) {
            
            block.push_back(get_key(line));            
            if (block.size() == block_size) {
                sort_func(block.data(), block.size()); //сортировка
                save_to_file(current_output);
                block.clear();
                current_output = (current_output == &out1) ? &out2 : &out1;
            }
        }
        //дозаписываем "хвост"
        if (block.size() > 0) {
            sort_func(block.data(), block.size()); //сортировка
            save_to_file(current_output);            
        }
    }

    //объединение файлов (ES2, ES3)
    static void merge_runs_balanced(const std::string& input1, const std::string& input2,
        const std::string& output1, const std::string& output2, size_t run_length) {

        std::ifstream in1(input1);
        std::ifstream in2(input2);
        std::ofstream out1(output1);
        std::ofstream out2(output2);

        if (!in1.is_open() || !in2.is_open() || !out1.is_open() || !out2.is_open()) {
            throw std::runtime_error("Cannot open files for merging");
        }

        std::ofstream* current_output = &out1;
        size_t run_counter = 0;
        
        while (has_data(in1) || has_data(in2)) {
            // Сливаем две серии длины run_length
            size_t merged = merge_two_runs_fixed_length(in1, in2, *current_output, run_length);

            if (merged == 0) {
                // Больше нет данных
                break;
            }            
            ++run_counter;
            // Переключаемся на другой выходной файл после каждой пары серий
            current_output = (run_counter % 2) ? &out2 : &out1;            
        }        
    }

    //объединение двух участков (ES2, ES3)
    static size_t merge_two_runs_fixed_length(std::ifstream& in1,
        std::ifstream& in2,
        std::ofstream& out,
        size_t run_length) {

        std::string line1, line2;
        size_t count1 = 0, count2 = 0;
        size_t total_written = 0;

        // Инициализируем: читаем первые элементы
        bool has1 = read_next_valid(in1, line1);
        bool has2 = read_next_valid(in2, line2);

        // Сливаем, пока есть данные в обеих сериях и не превысили длину
        while ((has1 && count1 < run_length) && (has2 && count2 < run_length)) {
            uint32_t val1 = get_key(line1);
            uint32_t val2 = get_key(line2);

            if (val1 <= val2) {
                out << line1 << '\n';
                ++total_written;
                has1 = (++count1 < run_length) ? read_next_valid(in1, line1) : false;
            }
            else {
                out << line2 << '\n';
                ++total_written;
                has2 = (++count2 < run_length) ? read_next_valid(in2, line2) : false;                
            }
        }

        // докопируем остатки из каждой серии до её длины
        // Первая серия
        while (has1 && count1 < run_length) {
            out << line1 << '\n';
            ++total_written;
            has1 = (++count1 < run_length) ? read_next_valid(in1, line1) : false;            
        }

        // Вторая серия
        while (has2 && count2 < run_length) {
            out << line2 << '\n';
            ++total_written;            
            has2 = (++count2 < run_length) ? read_next_valid(in2, line2) : false;
        }
        return total_written;
    }

    //считываем непустую строчку из файла (ES2, ES3)
    static bool read_next_valid(std::ifstream& file, std::string& line) {
        // Читаем пока не найдём непустую строку или не конец файла
        while (std::getline(file, line)) {
            if (!line.empty()) {
                return true;
            }
        }
        return false;
    }

    //подсчет записей в файле (ES2, ES3)
    static size_t count_records(const std::string& filename) {
        if (!std::filesystem::exists(filename)) {
            return 0;
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            return 0;
        }

        size_t count = 0;
        std::string line;

        while (std::getline(file, line)) {
            if (!line.empty()) {
                count++;
            }
        }

        return count;
    }

    //Простая проверка наличия данных (ES2, ES3)
    static bool has_data(std::ifstream& file) {
        // Проверяем, не достигли ли конца файла и есть ли что-то после текущей позиции
        return !file.eof() && file.peek() != EOF;
    }
};


