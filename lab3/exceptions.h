#pragma once
#include <stdexcept>
#include <string>

namespace RSAExceptions {
    
    class KeysNotGenerated : public std::logic_error {
    public:
        KeysNotGenerated() : std::logic_error("Ключи еще не сгенерированы. Сначала создайте или загрузите ключи.") {}
    };
    
    class InvalidKeyFile : public std::runtime_error {
    public:
        InvalidKeyFile(const std::string& filename) 
            : std::runtime_error("Невозможно прочитать файл ключа: " + filename) {}
    };
    
    class EncryptionError : public std::runtime_error {
    public:
        EncryptionError(const std::string& message) 
            : std::runtime_error("Ошибка шифрования: " + message) {}
    };
    
    class DecryptionError : public std::runtime_error {
    public:
        DecryptionError(const std::string& message) 
            : std::runtime_error("Ошибка расшифровки: " + message) {}
    };
    
    class FileReadError : public std::runtime_error {
    public:
        FileReadError(const std::string& filename) 
            : std::runtime_error("Не удалось прочитать файл: " + filename) {}
    };
    
    class FileWriteError : public std::runtime_error {
    public:
        FileWriteError(const std::string& filename) 
            : std::runtime_error("Не удалось записать файл: " + filename) {}
    };
    
    class FileTooLarge : public std::length_error {
    public:
        FileTooLarge(size_t size, size_t max_size) 
            : std::length_error("Файл слишком большой: " + std::to_string(size) 
                + " байт (максимум: " + std::to_string(max_size) + " байт)") {}
    };
    
    class PrimeGenerationError : public std::runtime_error {
    public:
        PrimeGenerationError() 
            : std::runtime_error("Ошибка генерации простого числа") {}
    };
    
    class InvalidModulusError : public std::logic_error {
    public:
        InvalidModulusError() 
            : std::logic_error("Неверный модуль: не может быть использован для RSA") {}
    };
}