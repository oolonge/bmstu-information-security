#pragma once
#include <cstdint>
#include <string>
#include <vector>

class DES {
public:
    DES();
    
    // Генерация и управление ключом
    void generate_key();
    void save_key(const std::string& path);
    void load_key(const std::string& path);
    void display_key() const;
    bool is_initialized() const { return key_generated; }
    
    // Шифрование и расшифровка файлов
    void encrypt_file(const std::string& input_path, const std::string& output_path);
    void decrypt_file(const std::string& input_path, const std::string& output_path);
    
private:
    uint64_t key;                    // 64-битный ключ (56 бит + 8 бит четности)
    uint64_t subkeys[16];            // 16 подключей для раундов
    bool key_generated;
    
    // Генерация подключей
    void generate_subkeys();
    
    // Основные операции DES
    uint64_t encrypt_block(uint64_t block);
    uint64_t decrypt_block(uint64_t block);
    
    // Вспомогательные функции
    uint64_t permute(uint64_t input, const int* table, int input_size, int output_size);
    uint32_t feistel(uint32_t R, uint64_t subkey);
    uint8_t sbox_lookup(uint8_t input, int box_num);
    uint64_t left_shift(uint64_t value, int shifts, int size);
    
    // Преобразование данных
    uint64_t bytes_to_uint64(const uint8_t* bytes);
    void uint64_to_bytes(uint64_t value, uint8_t* bytes);
};