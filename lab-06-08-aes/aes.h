#pragma once
#include <cstdint>
#include <string>
#include <vector>

class AES {
public:
    AES();
    
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
    uint8_t key[32];                 // 256-битный ключ
    uint32_t round_keys[60];         // 15 раундовых ключей по 4 слова (4*15 = 60)
    bool key_generated;
    
    // Генерация раундовых ключей
    void key_expansion();
    
    // Основные операции AES
    void encrypt_block(const uint8_t* input, uint8_t* output);
    void decrypt_block(const uint8_t* input, uint8_t* output);
    
    // Вспомогательные функции для раундов
    void sub_bytes(uint8_t state[4][4]);
    void inv_sub_bytes(uint8_t state[4][4]);
    void shift_rows(uint8_t state[4][4]);
    void inv_shift_rows(uint8_t state[4][4]);
    void mix_columns(uint8_t state[4][4]);
    void inv_mix_columns(uint8_t state[4][4]);
    void add_round_key(uint8_t state[4][4], int round);
    
    // Вспомогательные функции для Key Expansion
    uint32_t sub_word(uint32_t word);
    uint32_t rot_word(uint32_t word);
    
    // Умножение в поле Галуа для MixColumns
    uint8_t gmul(uint8_t a, uint8_t b);
    
    // Преобразование данных
    void bytes_to_state(const uint8_t* input, uint8_t state[4][4]);
    void state_to_bytes(const uint8_t state[4][4], uint8_t* output);
};