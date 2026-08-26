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
    uint64_t computed_hash_mod = computed_hash % n;

    // Расшифровка подписи публичным ключом
    uint64_t decrypted_hash = mod_pow(signature, e, n);

    // Сравнение хэшей (после модуля)
    bool is_valid = (computed_hash_mod == decrypted_hash);

    if (is_valid) {
        write_file(output_path, original_data);
        std::cout << "Подпись ВЕРНА.\n";
    } else {
        std::cout << "Подпись НЕВЕРНА!\n";
    }

    return is_valid;
}
