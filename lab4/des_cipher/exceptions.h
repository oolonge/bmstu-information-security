#pragma once
#include <stdexcept>
#include <string>

namespace DESExceptions {
    
    class KeyNotGenerated : public std::logic_error {
    public:
        KeyNotGenerated() : std::logic_error("Ключ еще не сгенерирован. Сначала создайте или загрузите ключ.") {}
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
    
    class InvalidBlockSize : public std::logic_error {
    public:
        InvalidBlockSize() 
            : std::logic_error("Неверный размер блока для DES (должен быть 64 бита)") {}
    };
}