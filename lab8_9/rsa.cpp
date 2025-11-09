#include "rsa.h"
#include "exceptions.h"
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <ctime>

RSA::RSA() : keys_generated(false), n(0), e(0), d(0), p(0), q(0) {}

void RSA::generate_keys() {
    std::cout << "Генерация пары ключей RSA...\n";

    // Генерация двух различных простых чисел
    std::random_device rd;
    std::mt19937_64 gen(rd());

    p = generate_prime(10000, 50000);
    do {
        q = generate_prime(10000, 50000);
    } while (q == p);

    // Вычисление модуля n = p * q
    n = p * q;

    // Вычисление функции Эйлера φ(n) = (p-1)(q-1)
    uint64_t phi = (p - 1) * (q - 1);

    // Выбор публичной экспоненты e (обычно 65537)
    e = 65537;
    if (e >= phi || gcd(e, phi) != 1) {
        // Если 65537 не подходит, ищем другое значение
        for (e = 3; e < phi; e += 2) {
            if (gcd(e, phi) == 1) {
                break;
            }
        }
    }

    // Вычисление приватной экспоненты d (обратное к e по модулю φ(n))
    d = mod_inverse(e, phi);

    keys_generated = true;

    std::cout << "Ключи успешно сгенерированы.\n";
    std::cout << "Размер модуля: " << n << "\n\n";
}

void RSA::save_keys(const std::string& public_path, const std::string& private_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeyNotGenerated();
    }

    // Сохранение публичного ключа
    std::ofstream pub_file(public_path, std::ios::binary);
    if (!pub_file) {
        throw RSAExceptions::FileWriteError(public_path);
    }

    pub_file.write(reinterpret_cast<const char*>(&n), sizeof(n));
    pub_file.write(reinterpret_cast<const char*>(&e), sizeof(e));
    pub_file.close();

    std::cout << "Публичный ключ сохранен: " << public_path << "\n";

    // Сохранение приватного ключа
    std::ofstream priv_file(private_path, std::ios::binary);
    if (!priv_file) {
        throw RSAExceptions::FileWriteError(private_path);
    }

    priv_file.write(reinterpret_cast<const char*>(&n), sizeof(n));
    priv_file.write(reinterpret_cast<const char*>(&d), sizeof(d));
    priv_file.write(reinterpret_cast<const char*>(&p), sizeof(p));
    priv_file.write(reinterpret_cast<const char*>(&q), sizeof(q));
    priv_file.close();

    std::cout << "Приватный ключ сохранен: " << private_path << "\n\n";
}

void RSA::load_keys(const std::string& public_path, const std::string& private_path) {
    // Загрузка публичного ключа
    std::ifstream pub_file(public_path, std::ios::binary);
    if (!pub_file) {
        throw RSAExceptions::InvalidKeyFile(public_path);
    }

    pub_file.read(reinterpret_cast<char*>(&n), sizeof(n));
    pub_file.read(reinterpret_cast<char*>(&e), sizeof(e));

    if (!pub_file) {
        pub_file.close();
        throw RSAExceptions::InvalidKeyFile(public_path);
    }
    pub_file.close();

    // Загрузка приватного ключа
    std::ifstream priv_file(private_path, std::ios::binary);
    if (!priv_file) {
        throw RSAExceptions::InvalidKeyFile(private_path);
    }

    uint64_t n_check;
    priv_file.read(reinterpret_cast<char*>(&n_check), sizeof(n_check));
    priv_file.read(reinterpret_cast<char*>(&d), sizeof(d));
    priv_file.read(reinterpret_cast<char*>(&p), sizeof(p));
    priv_file.read(reinterpret_cast<char*>(&q), sizeof(q));

    if (!priv_file || n_check != n) {
        priv_file.close();
        throw RSAExceptions::InvalidKeyFile(private_path);
    }
    priv_file.close();

    keys_generated = true;

    std::cout << "Ключи успешно загружены.\n";
    std::cout << "Публичный ключ: " << public_path << "\n";
    std::cout << "Приватный ключ: " << private_path << "\n\n";
}

void RSA::display_keys() const {
    if (!keys_generated) {
        std::cout << "Ключи еще не сгенерированы.\n\n";
        return;
    }

    std::cout << "\n=== Параметры RSA ===\n";
    std::cout << "Модуль (n):            " << n << "\n";
    std::cout << "Публичная экспонента (e): " << e << "\n";
    std::cout << "Приватная экспонента (d): " << d << "\n";
    std::cout << "Простое число p:       " << p << "\n";
    std::cout << "Простое число q:       " << q << "\n";
    std::cout << "Размер ключа (бит):    " << (sizeof(n) * 8) << "\n\n";
}

void RSA::sign_file(const std::string& input_path, const std::string& output_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeyNotGenerated();
    }

    // Чтение файла
    std::vector<uint8_t> file_data = read_file(input_path);

    // Вычисление хэша
    uint32_t hash = compute_hash(file_data);

    // Создание подписи (шифрование хэша приватным ключом)
    uint64_t signature = mod_pow(hash, d, n);

    // Подготовка данных для записи
    std::vector<uint8_t> output_data;

    // Запись размера подписи (8 байт для uint64_t)
    uint32_t sig_size = sizeof(signature);
    output_data.insert(output_data.end(),
                       reinterpret_cast<uint8_t*>(&sig_size),
                       reinterpret_cast<uint8_t*>(&sig_size) + sizeof(sig_size));

    // Запись подписи
    output_data.insert(output_data.end(),
                       reinterpret_cast<uint8_t*>(&signature),
                       reinterpret_cast<uint8_t*>(&signature) + sizeof(signature));

    // Запись оригинальных данных файла
    output_data.insert(output_data.end(), file_data.begin(), file_data.end());

    // Сохранение подписанного файла
    write_file(output_path, output_data);

    std::cout << "Файл подписан успешно.\n";
    std::cout << "Размер оригинала: " << file_data.size() << " байт\n";
    std::cout << "Размер с подписью: " << output_data.size() << " байт\n";
    std::cout << "Хэш файла: 0x" << std::hex << std::setw(8) << std::setfill('0')
              << hash << std::dec << "\n\n";
}

bool RSA::verify_file(const std::string& signed_path, const std::string& output_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeyNotGenerated();
    }

    // Чтение подписанного файла
    std::vector<uint8_t> signed_data = read_file(signed_path);

    // Проверка минимального размера
    if (signed_data.size() < sizeof(uint32_t) + sizeof(uint64_t)) {
        throw RSAExceptions::InvalidSignatureFormat();
    }

    // Извлечение размера подписи
    uint32_t sig_size;
    std::copy(signed_data.begin(),
              signed_data.begin() + sizeof(sig_size),
              reinterpret_cast<uint8_t*>(&sig_size));

    if (sig_size != sizeof(uint64_t)) {
        throw RSAExceptions::InvalidSignatureFormat();
    }

    // Извлечение подписи
    uint64_t signature;
    std::copy(signed_data.begin() + sizeof(uint32_t),
              signed_data.begin() + sizeof(uint32_t) + sizeof(signature),
              reinterpret_cast<uint8_t*>(&signature));

    // Извлечение оригинальных данных
    std::vector<uint8_t> original_data(
        signed_data.begin() + sizeof(uint32_t) + sizeof(uint64_t),
        signed_data.end()
    );

    // Вычисление хэша оригинальных данных
    uint32_t computed_hash = compute_hash(original_data);

    // Расшифровка подписи публичным ключом
    uint64_t decrypted_hash = mod_pow(signature, e, n);

    // Приведение к uint32_t для сравнения
    uint32_t decrypted_hash_32 = static_cast<uint32_t>(decrypted_hash);

    // Сравнение хэшей
    bool is_valid = (computed_hash == decrypted_hash_32);

    if (is_valid) {
        // Сохранение оригинального файла
        write_file(output_path, original_data);

        std::cout << "Подпись ВЕРНА.\n";
        std::cout << "Вычисленный хэш: 0x" << std::hex << std::setw(8) << std::setfill('0')
                  << computed_hash << std::dec << "\n";
        std::cout << "Хэш из подписи:  0x" << std::hex << std::setw(8) << std::setfill('0')
                  << decrypted_hash_32 << std::dec << "\n";
        std::cout << "Оригинальный файл восстановлен: " << output_path << "\n";
        std::cout << "Размер файла: " << original_data.size() << " байт\n\n";
    } else {
        std::cout << "Подпись НЕВЕРНА!\n";
        std::cout << "Вычисленный хэш: 0x" << std::hex << std::setw(8) << std::setfill('0')
                  << computed_hash << std::dec << "\n";
        std::cout << "Хэш из подписи:  0x" << std::hex << std::setw(8) << std::setfill('0')
                  << decrypted_hash_32 << std::dec << "\n";
        std::cout << "ВНИМАНИЕ: Файл был изменен или подпись повреждена!\n\n";
    }

    return is_valid;
}

// Проверка числа на простоту (метод Миллера-Рабина упрощенный)
bool RSA::is_prime(uint64_t num) const {
    if (num < 2) return false;
    if (num == 2 || num == 3) return true;
    if (num % 2 == 0) return false;

    // Проверка делением на малые простые числа
    for (uint64_t i = 3; i * i <= num && i < 1000; i += 2) {
        if (num % i == 0) return false;
    }

    return true;
}

// Генерация случайного простого числа в заданном диапазоне
uint64_t RSA::generate_prime(uint64_t min, uint64_t max) const {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(min, max);

    uint64_t candidate;
    do {
        candidate = dist(gen);
        if (candidate % 2 == 0) candidate++;  // Делаем нечетным
    } while (!is_prime(candidate));

    return candidate;
}

// НОД (алгоритм Евклида)
uint64_t RSA::gcd(uint64_t a, uint64_t b) const {
    while (b != 0) {
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Модульное обратное (расширенный алгоритм Евклида)
uint64_t RSA::mod_inverse(uint64_t a, uint64_t m) const {
    int64_t m0 = m;
    int64_t x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        int64_t q = a / m;
        int64_t t = m;

        m = a % m;
        a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;

    return x1;
}

// Модульное возведение в степень (быстрое возведение в степень)
uint64_t RSA::mod_pow(uint64_t base, uint64_t exp, uint64_t mod) const {
    uint64_t result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (__uint128_t)result * base % mod;
        }
        exp = exp >> 1;
        base = (__uint128_t)base * base % mod;
    }

    return result;
}

// Простая хэш-функция (DJB2)
uint32_t RSA::compute_hash(const std::vector<uint8_t>& data) const {
    uint32_t hash = 5381;

    for (uint8_t byte : data) {
        hash = ((hash << 5) + hash) + byte;  // hash * 33 + byte
    }

    return hash;
}

// Чтение файла в вектор байтов
std::vector<uint8_t> RSA::read_file(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw RSAExceptions::FileReadError(path);
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    if (!file) {
        throw RSAExceptions::FileReadError(path);
    }

    file.close();
    return buffer;
}

// Запись вектора байтов в файл
void RSA::write_file(const std::string& path, const std::vector<uint8_t>& data) const {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw RSAExceptions::FileWriteError(path);
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    if (!file) {
        throw RSAExceptions::FileWriteError(path);
    }

    file.close();
}
