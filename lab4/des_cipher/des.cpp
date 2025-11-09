#include "des.h"
#include "des_tables.h"
#include "exceptions.h"
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <cstring>

DES::DES() : key(0), key_generated(false) {
    std::memset(subkeys, 0, sizeof(subkeys));
}

void DES::generate_key() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;
    
    key = dist(gen);
    generate_subkeys();
    key_generated = true;
    
    std::cout << "Ключ успешно сгенерирован!\n";
}

void DES::save_key(const std::string& path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw DESExceptions::FileWriteError(path);
    }
    
    file.write(reinterpret_cast<const char*>(&key), sizeof(key));
    file.close();
    
    std::cout << "Ключ сохранен: " << path << "\n";
}

void DES::load_key(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw DESExceptions::InvalidKeyFile(path);
    }
    
    file.read(reinterpret_cast<char*>(&key), sizeof(key));
    
    if (!file) {
        throw DESExceptions::InvalidKeyFile(path);
    }
    
    file.close();
    
    generate_subkeys();
    key_generated = true;
    
    std::cout << "Ключ успешно загружен\n";
}

void DES::display_key() const {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::cout << "\n=== DES Ключ ===\n";
    std::cout << "Ключ (hex): " << std::hex << std::setw(16) << std::setfill('0') 
              << key << std::dec << "\n\n";
}

void DES::generate_subkeys() {
    // Применяем PC1 для получения 56-битного ключа
    uint64_t permuted_key = permute(key, DESTable::PC1, 64, 56);
    
    // Разделяем на две половины по 28 бит
    uint32_t C = (permuted_key >> 28) & 0x0FFFFFFF;
    uint32_t D = permuted_key & 0x0FFFFFFF;
    
    // Генерируем 16 подключей
    for (int i = 0; i < 16; ++i) {
        // Циклический сдвиг влево
        C = left_shift(C, DESTable::SHIFTS[i], 28);
        D = left_shift(D, DESTable::SHIFTS[i], 28);
        
        // Объединяем C и D
        uint64_t CD = ((uint64_t)C << 28) | D;
        
        // Применяем PC2 для получения 48-битного подключа
        subkeys[i] = permute(CD, DESTable::PC2, 56, 48);
    }
}

uint64_t DES::encrypt_block(uint64_t block) {
    // Initial Permutation
    block = permute(block, DESTable::IP, 64, 64);
    
    // Разделяем на левую и правую части
    uint32_t L = (block >> 32) & 0xFFFFFFFF;
    uint32_t R = block & 0xFFFFFFFF;
    
    // 16 раундов Фейстеля
    for (int i = 0; i < 16; ++i) {
        uint32_t temp = R;
        R = L ^ feistel(R, subkeys[i]);
        L = temp;
    }
    
    // Объединяем (R затем L для финальной перестановки)
    uint64_t combined = ((uint64_t)R << 32) | L;
    
    // Final Permutation
    return permute(combined, DESTable::FP, 64, 64);
}

uint64_t DES::decrypt_block(uint64_t block) {
    // Initial Permutation
    block = permute(block, DESTable::IP, 64, 64);
    
    // Разделяем на левую и правую части
    uint32_t L = (block >> 32) & 0xFFFFFFFF;
    uint32_t R = block & 0xFFFFFFFF;
    
    // 16 раундов Фейстеля (в обратном порядке)
    for (int i = 15; i >= 0; --i) {
        uint32_t temp = R;
        R = L ^ feistel(R, subkeys[i]);
        L = temp;
    }
    
    // Объединяем (R затем L)
    uint64_t combined = ((uint64_t)R << 32) | L;
    
    // Final Permutation
    return permute(combined, DESTable::FP, 64, 64);
}

uint32_t DES::feistel(uint32_t R, uint64_t subkey) {
    // Expansion (32 -> 48 бит)
    uint64_t expanded = permute(R, DESTable::E, 32, 48);
    
    // XOR с подключом
    uint64_t xored = expanded ^ subkey;
    
    // S-блоки (48 -> 32 бита)
    uint32_t output = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t input = (xored >> (42 - 6*i)) & 0x3F;  // Извлекаем 6 бит
        uint8_t sbox_out = sbox_lookup(input, i);
        output = (output << 4) | sbox_out;
    }
    
    // Permutation P
    return permute(output, DESTable::P, 32, 32);
}

uint8_t DES::sbox_lookup(uint8_t input, int box_num) {
    int row = ((input & 0x20) >> 4) | (input & 0x01);  // Биты 1 и 6
    int col = (input >> 1) & 0x0F;                      // Биты 2-5
    return DESTable::S[box_num][row][col];
}

uint64_t DES::permute(uint64_t input, const int* table, int input_size, int output_size) {
    uint64_t output = 0;
    for (int i = 0; i < output_size; ++i) {
        int bit_pos = table[i] - 1;  // Таблицы нумеруются с 1
        uint64_t bit = (input >> (input_size - 1 - bit_pos)) & 1;
        output = (output << 1) | bit;
    }
    return output;
}

uint64_t DES::left_shift(uint64_t value, int shifts, int size) {
    uint64_t mask = (1ULL << size) - 1;
    return ((value << shifts) | (value >> (size - shifts))) & mask;
}

void DES::encrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    // Открываем входной файл
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw DESExceptions::FileReadError(input_path);
    }
    
    // Получаем размер файла
    input.seekg(0, std::ios::end);
    uint64_t file_size = input.tellg();
    input.seekg(0, std::ios::beg);
    
    // Открываем выходной файл
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw DESExceptions::FileWriteError(output_path);
    }
    
    // Записываем оригинальный размер файла в начало
    output.write(reinterpret_cast<const char*>(&file_size), sizeof(file_size));
    
    std::cout << "Шифрование файла...\n";
    std::cout << "Размер файла: " << file_size << " байт\n";
    
    size_t blocks_processed = 0;
    size_t total_blocks = (file_size + 7) / 8;  // Округление вверх
    
    // Читаем и шифруем блоками по 8 байт
    uint8_t buffer[8];
    
    while (true) {
        input.read(reinterpret_cast<char*>(buffer), 8);
        size_t bytes_read = input.gcount();
        
        if (bytes_read == 0) break;
        
        // Zero padding для последнего блока
        if (bytes_read < 8) {
            std::memset(buffer + bytes_read, 0, 8 - bytes_read);
        }
        
        // Преобразуем в uint64_t и шифруем
        uint64_t block = bytes_to_uint64(buffer);
        uint64_t encrypted = encrypt_block(block);
        
        // Записываем зашифрованный блок
        uint8_t encrypted_bytes[8];
        uint64_to_bytes(encrypted, encrypted_bytes);
        output.write(reinterpret_cast<const char*>(encrypted_bytes), 8);
        
        blocks_processed++;
        if (blocks_processed % 1000 == 0 || blocks_processed == total_blocks) {
            std::cout << "Обработано блоков: " << blocks_processed << "/" << total_blocks << "\r" << std::flush;
        }
    }
    
    std::cout << "\nШифрование завершено успешно!\n";
    
    input.close();
    output.close();
}

void DES::decrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    // Открываем входной файл
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw DESExceptions::FileReadError(input_path);
    }
    
    // Читаем оригинальный размер файла
    uint64_t original_size;
    input.read(reinterpret_cast<char*>(&original_size), sizeof(original_size));
    
    if (!input) {
        throw DESExceptions::DecryptionError("Неверный формат зашифрованного файла");
    }
    
    // Открываем выходной файл
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw DESExceptions::FileWriteError(output_path);
    }
    
    std::cout << "Расшифровка файла...\n";
    std::cout << "Оригинальный размер: " << original_size << " байт\n";
    
    size_t blocks_processed = 0;
    uint64_t bytes_written = 0;
    
    // Читаем и расшифровываем блоками по 8 байт
    uint8_t buffer[8];
    
    while (input.read(reinterpret_cast<char*>(buffer), 8)) {
        // Преобразуем в uint64_t и расшифровываем
        uint64_t block = bytes_to_uint64(buffer);
        uint64_t decrypted = decrypt_block(block);
        
        // Преобразуем обратно в байты
        uint8_t decrypted_bytes[8];
        uint64_to_bytes(decrypted, decrypted_bytes);
        
        // Определяем сколько байт записать (для последнего блока)
        size_t bytes_to_write = 8;
        if (bytes_written + 8 > original_size) {
            bytes_to_write = original_size - bytes_written;
        }
        
        // Записываем расшифрованные байты
        output.write(reinterpret_cast<const char*>(decrypted_bytes), bytes_to_write);
        bytes_written += bytes_to_write;
        
        blocks_processed++;
        if (blocks_processed % 1000 == 0) {
            std::cout << "Обработано блоков: " << blocks_processed << "\r" << std::flush;
        }
    }
    
    std::cout << "\nРасшифровка завершена успешно!\n";
    
    input.close();
    output.close();
}

uint64_t DES::bytes_to_uint64(const uint8_t* bytes) {
    uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result = (result << 8) | bytes[i];
    }
    return result;
}

void DES::uint64_to_bytes(uint64_t value, uint8_t* bytes) {
    for (int i = 7; i >= 0; --i) {
        bytes[i] = value & 0xFF;
        value >>= 8;
    }
}