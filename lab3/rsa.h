#pragma once
#include <gmpxx.h>
#include <string>
#include <vector>
#include <cstdint>

class RSA {
public:
    RSA();
    
    // Генерация ключей
    void generate_keys(int bit_length = 512);
    
    // Сохранение и загрузка ключей
    void save_keys(const std::string& public_path, const std::string& private_path);
    void load_keys(const std::string& public_path, const std::string& private_path);
    
    // Шифрование и расшифровка файлов
    void encrypt_file(const std::string& input_path, const std::string& output_path);
    void decrypt_file(const std::string& input_path, const std::string& output_path);
    
    // Информация о ключах
    bool is_initialized() const { return keys_generated; }
    void display_public_key() const;
    size_t get_block_size() const;
    
private:
    // Криптографические параметры
    mpz_class n;  // Модуль (p * q)
    mpz_class e;  // Публичная экспонента
    mpz_class d;  // Приватная экспонента
    
    bool keys_generated;
    
    // Вспомогательные методы для генерации ключей
    mpz_class generate_prime(int bits);
    bool is_prime(const mpz_class& num, int iterations = 25);
    mpz_class gcd(const mpz_class& a, const mpz_class& b);
    mpz_class mod_inverse(const mpz_class& a, const mpz_class& m);
    
    // Шифрование/расшифровка блоков
    std::vector<uint8_t> encrypt_block(const std::vector<uint8_t>& block);
    std::vector<uint8_t> decrypt_block(const std::vector<uint8_t>& block);
    
    // Преобразование данных
    mpz_class bytes_to_mpz(const std::vector<uint8_t>& bytes);
    std::vector<uint8_t> mpz_to_bytes(const mpz_class& num, size_t size);
    
    // Размер блока в байтах (на 1 меньше размера модуля для безопасности)
    size_t get_max_block_size() const;
};