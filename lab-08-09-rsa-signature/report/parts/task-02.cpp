void RSA::sign_file(const std::string& input_path, const std::string& output_path) {
    if (!keys_generated) {
        throw RSAExceptions::KeyNotGenerated();
    }

    // Чтение файла
    std::vector<uint8_t> file_data = read_file(input_path);

    // Вычисление хэша
    uint32_t hash = compute_hash(file_data);

    // Приведение хэша к диапазону модуля (чтобы hash < n)
    uint64_t hash_mod = hash % n;

    // Создание подписи (шифрование хэша приватным ключом)
    uint64_t signature = mod_pow(hash_mod, d, n);

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
}
