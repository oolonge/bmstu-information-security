#include "rsa.h"
#include "exceptions.h"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <iomanip>

RSA::RSA() : keys_generated(false) {
    // Инициализация генератора случайных чисел GMP
    gmp_randclass rng(gmp_randinit_default);
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

void RSA::generate_keys(int bit_length) {
    std::cout << "Генерация ключей RSA (" << bit_length << " бит)...\n";
    
    // Генерируем два простых числа
    std::cout << "Генерация первого простого числа p..." << std::flush;
    mpz_class p = generate_prime(bit_length / 2);
    std::cout << " готово\n";
    
    std::cout << "Генерация второго простого числа q..." << std::flush;
    mpz_class q = generate_prime(bit_length / 2);
    std::cout << " готово\n";
    
    // Вычисляем n = p * q
    n = p * q;
    
    // Вычисляем функцию Эйлера φ(n) = (p-1)(q-1)
    mpz_class phi = (p - 1) * (q - 1);
    
    // Выбираем публичную экспоненту e (обычно 65537)
    e = 65537;
    
    // Проверяем, что gcd(e, phi) = 1
    if (gcd(e, phi) != 1) {
        e = 3;
        while (gcd(e, phi) != 1) {
            e += 2;
        }
    }
    
    // Вычисляем приватную экспоненту d
    std::cout << "Вычисление приватного ключа..." << std::flush;
    d = mod_inverse(e, phi);
    std::cout << " готово\n";
    
    keys_generated = true;
    std::cout << "Ключи успешно сгенерированы!\n";
}

mpz_class RSA::generate_prime(int bits) {
    gmp_randclass rng(gmp_randinit_default);
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
    
    mpz_class candidate;
    int max_attempts = 1000;
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        // Генерируем случайное нечетное число нужного размера
        candidate = rng.get_z_bits(bits);
        
        // Устанавливаем старший бит в 1 (для правильного размера)
        mpz_setbit(candidate.get_mpz_t(), bits - 1);
        
        // Делаем число нечетным
        if (mpz_even_p(candidate.get_mpz_t())) {
            candidate += 1;
        }
        
        // Проверяем на простоту
        if (is_prime(candidate)) {
            return candidate;
        }
    }
    
    throw RSAExceptions::PrimeGenerationError();
}

bool RSA::is_prime(const mpz_class& num, int iterations) {
    // Используем встроенный тест Миллера-Рабина из GMP
    return mpz_probab_prime_p(num.get_mpz_t(), iterations) > 0;
}

mpz_class RSA::gcd(const mpz_class& a, const mpz_class& b) {
    mpz_class result;
    mpz_gcd(result.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
    return result;
}

mpz_class RSA::mod_inverse(const mpz_class& a, const mpz_class& m) {
    mpz_class result;
    
    // Используем встроенную функцию GMP для нахождения обратного по модулю
    int success = mpz_invert(result.get_mpz_t(), a.get_mpz_t(), m.get_mpz_t());
    
    if (success == 0) {
        throw RSAExceptions::InvalidModulusError();
    }
    
    return result;
}

void RSA::save_keys(const std::string& public_path, const std::string& private_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeysNotGenerated();
    }
    
    // Сохраняем публичный ключ
    std::ofstream pub_file(public_path);
    if (!pub_file.is_open()) {
        throw RSAExceptions::FileWriteError(public_path);
    }
    
    pub_file << "n=" << n.get_str(16) << "\n";
    pub_file << "e=" << e.get_str(16) << "\n";
    pub_file.close();
    
    // Сохраняем приватный ключ
    std::ofstream priv_file(private_path);
    if (!priv_file.is_open()) {
        throw RSAExceptions::FileWriteError(private_path);
    }
    
    priv_file << "n=" << n.get_str(16) << "\n";
    priv_file << "d=" << d.get_str(16) << "\n";
    priv_file.close();
    
    std::cout << "Ключи сохранены:\n";
    std::cout << "  Публичный: " << public_path << "\n";
    std::cout << "  Приватный: " << private_path << "\n";
}

void RSA::load_keys(const std::string& public_path, const std::string& private_path) {
    // Загружаем публичный ключ
    std::ifstream pub_file(public_path);
    if (!pub_file.is_open()) {
        throw RSAExceptions::InvalidKeyFile(public_path);
    }
    
    std::string line;
    std::string n_str, e_str;
    
    while (std::getline(pub_file, line)) {
        if (line.substr(0, 2) == "n=") {
            n_str = line.substr(2);
        } else if (line.substr(0, 2) == "e=") {
            e_str = line.substr(2);
        }
    }
    pub_file.close();
    
    // Загружаем приватный ключ
    std::ifstream priv_file(private_path);
    if (!priv_file.is_open()) {
        throw RSAExceptions::InvalidKeyFile(private_path);
    }
    
    std::string d_str;
    while (std::getline(priv_file, line)) {
        if (line.substr(0, 2) == "d=") {
            d_str = line.substr(2);
        }
    }
    priv_file.close();
    
    // Парсим значения
    if (n_str.empty() || e_str.empty() || d_str.empty()) {
        throw RSAExceptions::InvalidKeyFile("Неверный формат файлов ключей");
    }
    
    n.set_str(n_str, 16);
    e.set_str(e_str, 16);
    d.set_str(d_str, 16);
    
    keys_generated = true;
    std::cout << "Ключи успешно загружены\n";
}

void RSA::display_public_key() const {
    if (!keys_generated) {
        throw RSAExceptions::KeysNotGenerated();
    }
    
    std::cout << "\n=== Публичный ключ ===\n";
    std::cout << "Модуль n (" << mpz_sizeinbase(n.get_mpz_t(), 2) << " бит):\n";
    std::cout << n.get_str(16) << "\n\n";
    std::cout << "Экспонента e:\n";
    std::cout << e.get_str(10) << "\n\n";
}

size_t RSA::get_block_size() const {
    return get_max_block_size();
}

size_t RSA::get_max_block_size() const {
    // Размер блока на 1 байт меньше размера модуля
    size_t n_bytes = (mpz_sizeinbase(n.get_mpz_t(), 2) + 7) / 8;
    return n_bytes - 1;
}

void RSA::encrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeysNotGenerated();
    }
    
    // Открываем входной файл
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw RSAExceptions::FileReadError(input_path);
    }
    
    // Получаем размер файла
    input.seekg(0, std::ios::end);
    size_t file_size = input.tellg();
    input.seekg(0, std::ios::beg);
    
    // Открываем выходной файл
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw RSAExceptions::FileWriteError(output_path);
    }
    
    size_t block_size = get_max_block_size();
    size_t encrypted_block_size = (mpz_sizeinbase(n.get_mpz_t(), 2) + 7) / 8;
    
    // Записываем размер зашифрованного блока в начало файла
    uint32_t enc_block_size_header = static_cast<uint32_t>(encrypted_block_size);
    output.write(reinterpret_cast<const char*>(&enc_block_size_header), sizeof(enc_block_size_header));
    
    std::cout << "Шифрование файла...\n";
    std::cout << "Размер файла: " << file_size << " байт\n";
    std::cout << "Размер блока: " << block_size << " байт\n";
    
    size_t blocks_processed = 0;
    size_t total_blocks = (file_size + block_size - 1) / block_size;
    
    // Читаем и шифруем блоками
    std::vector<uint8_t> buffer(block_size);
    
    while (input.read(reinterpret_cast<char*>(buffer.data()), block_size) || input.gcount() > 0) {
        size_t bytes_read = input.gcount();
        
        // Подготавливаем блок нужного размера
        std::vector<uint8_t> block(buffer.begin(), buffer.begin() + bytes_read);
        
        // Шифруем блок
        std::vector<uint8_t> encrypted = encrypt_block(block);
        
        // Записываем зашифрованный блок
        output.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
        
        blocks_processed++;
        if (blocks_processed % 100 == 0 || blocks_processed == total_blocks) {
            std::cout << "Обработано блоков: " << blocks_processed << "/" << total_blocks << "\r" << std::flush;
        }
    }
    
    std::cout << "\nШифрование завершено успешно!\n";
    
    input.close();
    output.close();
}

void RSA::decrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeysNotGenerated();
    }
    
    // Открываем входной файл
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw RSAExceptions::FileReadError(input_path);
    }
    
    // Читаем размер зашифрованного блока из заголовка
    uint32_t encrypted_block_size;
    input.read(reinterpret_cast<char*>(&encrypted_block_size), sizeof(encrypted_block_size));
    
    if (!input) {
        throw RSAExceptions::DecryptionError("Неверный формат зашифрованного файла");
    }
    
    // Открываем выходной файл
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw RSAExceptions::FileWriteError(output_path);
    }
    
    std::cout << "Расшифровка файла...\n";
    std::cout << "Размер зашифрованного блока: " << encrypted_block_size << " байт\n";
    
    size_t blocks_processed = 0;
    
    // Читаем и расшифровываем блоками
    std::vector<uint8_t> buffer(encrypted_block_size);
    
    while (input.read(reinterpret_cast<char*>(buffer.data()), encrypted_block_size)) {
        // Расшифровываем блок
        std::vector<uint8_t> decrypted = decrypt_block(buffer);
        
        // Записываем расшифрованный блок
        output.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
        
        blocks_processed++;
        if (blocks_processed % 100 == 0) {
            std::cout << "Обработано блоков: " << blocks_processed << "\r" << std::flush;
        }
    }
    
    std::cout << "\nРасшифровка завершена успешно!\n";
    
    input.close();
    output.close();
}

std::vector<uint8_t> RSA::encrypt_block(const std::vector<uint8_t>& block) {
    // Преобразуем байты в большое число
    mpz_class m = bytes_to_mpz(block);
    
    // Проверяем, что m < n
    if (m >= n) {
        throw RSAExceptions::EncryptionError("Блок данных больше модуля");
    }
    
    // Шифруем: c = m^e mod n
    mpz_class c;
    mpz_powm(c.get_mpz_t(), m.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
    
    // Преобразуем обратно в байты
    size_t encrypted_size = (mpz_sizeinbase(n.get_mpz_t(), 2) + 7) / 8;
    return mpz_to_bytes(c, encrypted_size);
}

std::vector<uint8_t> RSA::decrypt_block(const std::vector<uint8_t>& block) {
    // Преобразуем байты в большое число
    mpz_class c = bytes_to_mpz(block);
    
    // Расшифровываем: m = c^d mod n
    mpz_class m;
    mpz_powm(m.get_mpz_t(), c.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
    
    // Преобразуем обратно в байты (без дополнительных нулей)
    size_t decrypted_size = (mpz_sizeinbase(m.get_mpz_t(), 2) + 7) / 8;
    if (decrypted_size == 0) decrypted_size = 1;
    
    return mpz_to_bytes(m, decrypted_size);
}

mpz_class RSA::bytes_to_mpz(const std::vector<uint8_t>& bytes) {
    mpz_class result = 0;
    
    for (size_t i = 0; i < bytes.size(); ++i) {
        result = (result << 8) | bytes[i];
    }
    
    return result;
}

std::vector<uint8_t> RSA::mpz_to_bytes(const mpz_class& num, size_t size) {
    std::vector<uint8_t> bytes(size);
    
    mpz_class temp = num;
    for (int i = size - 1; i >= 0; --i) {
        bytes[i] = mpz_get_ui(temp.get_mpz_t()) & 0xFF;
        temp >>= 8;
    }
    
    return bytes;
}