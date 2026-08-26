#pragma once
#include <stdexcept>
#include <string>

namespace RSAExceptions {

    class KeyNotGenerated : public std::logic_error {
    public:
        KeyNotGenerated() : std::logic_error("Ключи еще не сгенерированы. Сначала создайте или загрузите ключи.") {}
    };

    class InvalidKeyFile : public std::runtime_error {
    public:
        InvalidKeyFile(const std::string& filename)
            : std::runtime_error("Невозможно прочитать файл ключа: " + filename) {}
    };

    class SignatureError : public std::runtime_error {
    public:
        SignatureError(const std::string& message)
            : std::runtime_error("Ошибка создания подписи: " + message) {}
    };

    class VerificationError : public std::runtime_error {
    public:
        VerificationError(const std::string& message)
            : std::runtime_error("Ошибка проверки подписи: " + message) {}
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

    class InvalidSignatureFormat : public std::runtime_error {
    public:
        InvalidSignatureFormat()
            : std::runtime_error("Неверный формат подписанного файла") {}
    };
}
