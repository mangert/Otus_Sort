#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <random>

class ExternalSorter {
public:
	
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


};


