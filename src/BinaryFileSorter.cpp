#pragma once
#include <iostream>
#include <fstream>
#include <cstdint>
#include <random>
#include <string>

class BinaryFileSorter {
private:
    std::string input_filename;
    std::string output_filename;

public:
    
    BinaryFileSorter(const std::string& input, const std::string& output)
        : input_filename(input), output_filename(output) {
    }

    // Генерация бинарного файла со случайными 16-битными числами
    void generate_random_file(uint64_t n) {
     
        std::cout << "Генерация файла с " << n << " числами ("
            << (n * 2) / (1024.0 * 1024.0) << " МБ)...\n";
        

        std::ofstream file(input_filename, std::ios::binary | std::ios::trunc);
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
                // Разбиваем 16-битное число на 2 байта
                char bytes[2];
                bytes[0] = static_cast<char>(buffer[i] & 0xFF);        // Младший байт
                bytes[1] = static_cast<char>((buffer[i] >> 8) & 0xFF); // Старший байт

                file.write(bytes, 2);
            }

            if (!file) {
                std::cerr << "Ошибка записи!\n";
                return;
            }

            numbers_written += block_size;
        }
    }
};