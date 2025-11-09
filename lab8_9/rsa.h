#pragma once
#include <cstdint>
#include <string>
#include <vector>

class RSA {
public:
    RSA();

    // Управление ключами
    void generate_keys();
    void save_keys(const std::string& public_path, const std::string& private_path);
    void load_keys(const std::string& public_path, const std::string& private_path);
    void display_keys() const;
    bool is_initialized() const { return keys_generated; }

    // Операции с подписью
    void sign_file(const std::string& input_path, const std::string& output_path);
    bool verify_file(const std::string& signed_path, const std::string& output_path);

private:
    bool keys_generated;

    // RSA ключи
    uint64_t n;  // Модуль
    uint64_t e;  // Публичная экспонента
    uint64_t d;  // Приватная экспонента
    uint64_t p;  // Простое число 1
    uint64_t q;  // Простое число 2

    // Внутренние методы RSA
    bool is_prime(uint64_t num) const;
    uint64_t generate_prime(uint64_t min, uint64_t max) const;
    uint64_t gcd(uint64_t a, uint64_t b) const;
    uint64_t mod_inverse(uint64_t a, uint64_t m) const;
    uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) const;

    // Хэширование
    uint32_t compute_hash(const std::vector<uint8_t>& data) const;
    std::vector<uint8_t> read_file(const std::string& path) const;
    void write_file(const std::string& path, const std::vector<uint8_t>& data) const;
};
