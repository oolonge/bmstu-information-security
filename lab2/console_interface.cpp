#include "console_interface.h"
#include "exceptions.h"

ConsoleInterface::ConsoleInterface() {}

void ConsoleInterface::run() {
    while (true) {
        display_main_menu();
        
        int choice;
        std::cout << "> ";
        
        if (!get_valid_integer(choice, 0, 4)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 4.\n";
            std::cout << "Попробуйте снова.\n\n";
            continue;
        }
        
        if (choice == 0) {
            break;
        }
        
        switch (choice) {
            case 1:
                handle_encrypt_message();
                break;
            case 2:
                handle_change_configuration();
                break;
            case 3:
                handle_display_configuration();
                break;
            case 4:
                handle_reset_machine();
                break;
            default:
                std::cout << "Неверный выбор. Попробуйте снова.\n\n";
        }
    }
}

void ConsoleInterface::display_main_menu() {
    std::cout << "=== Машина Энигма ===\n";
    std::cout << "1. Ввести сообщение\n";
    std::cout << "2. Изменить конфигурацию\n";
    std::cout << "3. Посмотреть конфигурацию\n";
    std::cout << "4. Сбросить машину к начальным позициям\n";
    std::cout << "0. Выход\n";
}

void ConsoleInterface::handle_encrypt_message() {
    if (!machine.is_initialized()) {
        std::cout << "Машина ещё не настроена. Сначала настройте конфигурацию\n\n";
        return;
    }
    
    handle_message_input_choice();
}

void ConsoleInterface::handle_message_input_choice() {
    while (true) {
        std::cout << "\nВыберите способ ввода данных:\n";
        std::cout << "1. Прочитать текстовое сообщение из файла (.txt)\n";
        std::cout << "2. Ввести сообщение с клавиатуры\n";
        std::cout << "3. Работа с архивом\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "Ваш выбор: ";
        
        int choice;
        if (!get_valid_integer(choice, 0, 3)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 3.\n";
            std::cout << "Попробуйте снова.\n";
            continue;
        }
        
        if (choice == 0) {
            return;
        }
        
        try {
            switch (choice) {
                case 1: {
                    std::string message = read_message_from_file();
                    if (!message.empty()) {
                        std::string result = machine.encrypt_message(message);
                        std::cout << "\nРезультат: " << result << "\n";
                        handle_output_choice(result);
                    }
                    return;
                }
                case 2: {
                    std::string message = read_message_from_keyboard();
                    if (!message.empty()) {
                        std::string result = machine.encrypt_message(message);
                        std::cout << "\nРезультат: " << result << "\n";
                        handle_output_choice(result);
                    }
                    return;
                }
                case 3: {
                    handle_archive_operations();
                    return;
                }
            }
            
        } catch (const std::exception& e) {
            std::cout << "Ошибка при шифровании: " << e.what() << "\n\n";
            return;
        }
    }
}

void ConsoleInterface::handle_archive_operations() {
    while (true) {
        std::cout << "\nВыберите операцию с архивом:\n";
        std::cout << "1. Зашифровать архив\n";
        std::cout << "2. Расшифровать архив\n";
        std::cout << "0. Вернуться в предыдущее меню\n";
        std::cout << "Ваш выбор: ";
        
        int choice;
        if (!get_valid_integer(choice, 0, 2)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 2.\n";
            std::cout << "Попробуйте снова.\n";
            continue;
        }
        
        if (choice == 0) {
            return;
        }
        
        try {
            switch (choice) {
                case 1:
                    handle_archive_encryption();
                    return;
                case 2:
                    handle_archive_decryption();
                    return;
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка при работе с архивом: " << e.what() << "\n\n";
            return;
        }
    }
}

void ConsoleInterface::handle_archive_encryption() {
    std::vector<std::string> archive_files = get_archive_files_in_out_directory();
    
    if (archive_files.empty()) {
        std::cout << "В директории out/ не найдено архивных файлов (.zip, .rar, .7z, .tar, .gz)\n\n";
        return;
    }
    
    display_available_archives(archive_files);
    
    std::cout << "Выберите архив (введите номер от 1 до " << archive_files.size() << "): ";
    int file_choice;
    
    if (!get_valid_integer(file_choice, 1, static_cast<int>(archive_files.size()))) {
        throw std::runtime_error("Неверный выбор файла");
    }
    
    std::string selected_file = archive_files[file_choice - 1];
    std::cout << "Выбран файл: " << selected_file << "\n";
    
    // Сброс машины к начальным позициям перед шифрованием
    machine.reset_to_initial_positions();
    
    // Чтение бинарного файла
    std::string filepath = "out/" + selected_file;
    std::vector<uint8_t> file_data = read_binary_file(filepath);
    std::cout << "Файл прочитан (" << file_data.size() << " байт)\n";
    
    // Шифрование
    std::cout << "Выполняется шифрование...\n";
    std::vector<uint8_t> encrypted_data = machine.encrypt_binary_data(file_data);
    
    // Формирование имени выходного файла
    std::string output_filename = "out/" + selected_file + "_encoded";
    
    // Запись зашифрованного файла
    write_binary_file(output_filename, encrypted_data);
    
    std::cout << "Архив успешно зашифрован и сохранен как: " << output_filename << "\n";
    std::cout << "Размер зашифрованного файла: " << encrypted_data.size() << " байт\n\n";
}

void ConsoleInterface::handle_archive_decryption() {
    std::vector<std::string> encoded_files = get_encoded_files_in_out_directory();
    
    if (encoded_files.empty()) {
        std::cout << "В директории out/ не найдено зашифрованных файлов (с окончанием _encoded)\n\n";
        return;
    }
    
    display_available_encoded_files(encoded_files);
    
    std::cout << "Выберите зашифрованный файл (введите номер от 1 до " << encoded_files.size() << "): ";
    int file_choice;
    
    if (!get_valid_integer(file_choice, 1, static_cast<int>(encoded_files.size()))) {
        throw std::runtime_error("Неверный выбор файла");
    }
    
    std::string selected_file = encoded_files[file_choice - 1];
    std::cout << "Выбран файл: " << selected_file << "\n";
    
    // Сброс машины к начальным позициям перед расшифровкой
    machine.reset_to_initial_positions();
    
    // Чтение зашифрованного файла
    std::string filepath = "out/" + selected_file;
    std::vector<uint8_t> encrypted_data = read_binary_file(filepath);
    std::cout << "Зашифрованный файл прочитан (" << encrypted_data.size() << " байт)\n";
    
    // Расшифровка (Энигма симметрична - шифрование = расшифровке)
    std::cout << "Выполняется расшифровка...\n";
    std::vector<uint8_t> decrypted_data = machine.encrypt_binary_data(encrypted_data);
    
    // Формирование имени выходного файла
    std::string original_name = selected_file;
    // Убираем "_encoded" из имени
    size_t encoded_pos = original_name.find("_encoded");
    if (encoded_pos != std::string::npos) {
        original_name = original_name.substr(0, encoded_pos);
    }
    
    // Добавляем "_decoded" перед расширением
    size_t dot_pos = original_name.find_last_of('.');
    std::string output_filename;
    if (dot_pos != std::string::npos) {
        output_filename = "out/" + original_name.substr(0, dot_pos) + "_decoded" + original_name.substr(dot_pos);
    } else {
        output_filename = "out/" + original_name + "_decoded";
    }
    
    // Запись расшифрованного файла
    write_binary_file(output_filename, decrypted_data);
    
    std::cout << "Архив успешно расшифрован и сохранен как: " << output_filename << "\n";
    std::cout << "Размер расшифрованного файла: " << decrypted_data.size() << " байт\n\n";
}

std::vector<uint8_t> ConsoleInterface::read_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filepath);
    }
    
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(file_size);
    file.read(reinterpret_cast<char*>(data.data()), file_size);
    
    if (!file) {
        throw std::runtime_error("Ошибка при чтении файла: " + filepath);
    }
    
    file.close();
    return data;
}

void ConsoleInterface::write_binary_file(const std::string& filepath, const std::vector<uint8_t>& data) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать файл: " + filepath);
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (!file) {
        throw std::runtime_error("Ошибка при записи файла: " + filepath);
    }
    
    file.close();
}

std::vector<std::string> ConsoleInterface::get_archive_files_in_out_directory() {
    std::vector<std::string> archive_files;
    std::vector<std::string> extensions = {".zip", ".rar", ".7z", ".tar", ".gz", ".bz2"};
    
    try {
        if (!std::filesystem::exists("out/")) {
            return archive_files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("out/")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Пропускаем файлы с _encoded
                if (filename.find("_encoded") != std::string::npos) {
                    continue;
                }
                
                // Преобразуем расширение к нижнему регистру
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    archive_files.push_back(filename);
                }
            }
        }
        
        // Сортируем файлы по имени
        std::sort(archive_files.begin(), archive_files.end());
        
    } catch (const std::exception&) {
        // В случае ошибки возвращаем пустой вектор
        archive_files.clear();
    }
    
    return archive_files;
}

std::vector<std::string> ConsoleInterface::get_encoded_files_in_out_directory() {
    std::vector<std::string> encoded_files;
    
    try {
        if (!std::filesystem::exists("out/")) {
            return encoded_files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("out/")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Ищем файлы с окончанием "_encoded"
                if (filename.find("_encoded") != std::string::npos) {
                    encoded_files.push_back(filename);
                }
            }
        }
        
        // Сортируем файлы по имени
        std::sort(encoded_files.begin(), encoded_files.end());
        
    } catch (const std::exception&) {
        // В случае ошибки возвращаем пустой вектор
        encoded_files.clear();
    }
    
    return encoded_files;
}

void ConsoleInterface::display_available_archives(const std::vector<std::string>& files) {
    std::cout << "\nДоступные архивные файлы в директории out/:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << files[i] << "\n";
    }
    std::cout << "\n";
}

void ConsoleInterface::display_available_encoded_files(const std::vector<std::string>& files) {
    std::cout << "\nДоступные зашифрованные файлы в директории out/:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << files[i] << "\n";
    }
    std::cout << "\n";
}

std::string ConsoleInterface::read_message_from_keyboard() {
    std::string message;
    std::cout << "Введите сообщение: ";
    std::getline(std::cin, message);
    return message;
}

std::string ConsoleInterface::read_message_from_file() {
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать или получить доступ к директории out/");
    }
    
    std::vector<std::string> txt_files = get_txt_files_in_out_directory();
    
    if (txt_files.empty()) {
        throw std::runtime_error("В директории out/ не найдено .txt файлов");
    }
    
    display_available_files(txt_files);
    
    std::cout << "Выберите файл (введите номер от 1 до " << txt_files.size() << "): ";
    int file_choice;
    
    if (!get_valid_integer(file_choice, 1, static_cast<int>(txt_files.size()))) {
        throw std::runtime_error("Неверный выбор файла");
    }
    
    std::string selected_file = txt_files[file_choice - 1];
    std::string filepath = "out/" + selected_file;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filepath);
    }
    
    std::string message;
    std::string line;
    
    while (std::getline(file, line)) {
        message += line;
        if (!file.eof()) {
            message += "\n";
        }
    }
    
    file.close();
    
    if (message.empty()) {
        throw std::runtime_error("Файл пуст или не содержит читаемого текста");
    }
    
    std::cout << "Сообщение успешно прочитано из файла " << selected_file 
              << " (" << message.length() << " символов)\n";
    return message;
}

void ConsoleInterface::handle_output_choice(const std::string& encrypted_message) {
    while (true) {
        std::cout << "\nВыберите действие:\n";
        std::cout << "1. Записать результат в файл\n";
        std::cout << "2. Выйти в меню\n";
        std::cout << "Ваш выбор: ";
        
        int choice;
        if (!get_valid_integer(choice, 1, 2)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 1 до 2.\n";
            std::cout << "Попробуйте снова.\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                try {
                    write_message_to_file(encrypted_message);
                } catch (const std::exception& e) {
                    std::cout << "Ошибка при записи в файл: " << e.what() << "\n";
                }
                return;
            case 2:
                return;
        }
    }
}

void ConsoleInterface::write_message_to_file(const std::string& message) {
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать директорию out/");
    }
    
    std::string filename;
    std::cout << "Введите название файла (без расширения): ";
    std::getline(std::cin, filename);
    
    if (filename.empty()) {
        throw std::runtime_error("Имя файла не может быть пустым");
    }
    
    // Очистка имени файла от недопустимых символов
    std::string cleaned_filename;
    for (char c : filename) {
        if (std::isalnum(c) || c == '_' || c == '-') {
            cleaned_filename += c;
        } else if (c == ' ') {
            cleaned_filename += '_';
        }
    }
    
    if (cleaned_filename.empty()) {
        cleaned_filename = "encrypted_message";
    }
    
    std::string filepath = "out/" + cleaned_filename + ".txt";
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать файл: " + filepath);
    }
    
    file << message;
    file.close();
    
    std::cout << "Результат успешно записан в файл: " << filepath << "\n\n";
}

bool ConsoleInterface::create_output_directory() {
    try {
        std::filesystem::create_directories("out");
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> ConsoleInterface::get_txt_files_in_out_directory() {
    std::vector<std::string> txt_files;
    
    try {
        if (!std::filesystem::exists("out/")) {
            return txt_files; // Возвращаем пустой вектор, если директория не существует
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("out/")) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                txt_files.push_back(entry.path().filename().string());
            }
        }
        
        // Сортируем файлы по имени для стабильного порядка
        std::sort(txt_files.begin(), txt_files.end());
        
    } catch (const std::exception&) {
        // В случае ошибки возвращаем пустой вектор
        txt_files.clear();
    }
    
    return txt_files;
}

void ConsoleInterface::display_available_files(const std::vector<std::string>& files) {
    std::cout << "\nДоступные .txt файлы в директории out/:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << files[i] << "\n";
    }
    std::cout << "\n";
}

void ConsoleInterface::handle_change_configuration() {
    if (machine.is_initialized()) {
        modify_existing_machine();
    } else {
        configure_new_machine();
    }
}

void ConsoleInterface::handle_display_configuration() {
    machine.display_configuration();
}

void ConsoleInterface::handle_reset_machine() {
    if (!machine.is_initialized()) {
        std::cout << "Машина ещё не настроена. Сначала настройте конфигурацию\n\n";
        return;
    }
    
    machine.reset_to_initial_positions();
    std::cout << "Машина сброшена к начальным позициям.\n\n";
}

void ConsoleInterface::display_available_rotors() {
    std::cout << "\nДоступные роторы:\n";
    
    const auto& rotor_configs = Enigma::get_rotor_configs();

    std::vector<std::pair<uint8_t, std::pair<std::string, char>>> sorted_rotors;
    for (const auto& pair : rotor_configs) {
        sorted_rotors.push_back(pair);
    }
    
    std::sort(sorted_rotors.begin(), sorted_rotors.end(), 
              [](const auto& a, const auto& b) {
                  return a.first < b.first;
              });
    
    for (const auto& pair : sorted_rotors) {
        uint8_t rotor_num = pair.first;
        const std::string& wiring = pair.second.first;
        char notch = pair.second.second;
        
        std::cout << "  Ротор " << static_cast<int>(rotor_num) << ": " 
                  << wiring << " | Notch: " << notch << "\n";
    }
    std::cout << "\n";
}

void ConsoleInterface::configure_new_machine() {
    machine = Enigma(); // Создаем новую машину
    
    display_available_rotors();
    
    std::vector<uint8_t> rotors;
    std::string rings;
    char reflector_type;
    
    if (!input_rotors(rotors)) {
        return;
    }
    
    if (!input_rings(rotors.size(), rings)) {
        return;
    }
    
    if (!input_reflector(reflector_type)) {
        return;
    }
    
    try {
        machine.setup(rotors, rings, reflector_type);
        std::cout << "Машина настроена!\n";
        machine.display_configuration();
    } catch (const std::exception& e) {
        std::cout << "Ошибка при настройке машины: " << e.what() << "\n\n";
    }
}

void ConsoleInterface::configure_rotors_only() {
    display_available_rotors();
    
    std::vector<uint8_t> rotors;
    input_rotors(rotors);
    
    try {
        machine.setup_rotors_only(rotors);
        std::cout << "Роторы изменены!\n";
        machine.display_configuration();
    } catch (const std::exception& e) {
        std::cout << "Ошибка при изменении роторов: " << e.what() << "\n\n";
    }
}

void ConsoleInterface::modify_existing_machine_partial() {
    while (true) {
        std::cout << "\nТекущая конфигурация машины:\n";
        machine.display_configuration();
        
        std::cout << "Выберите что изменить:\n";
        std::cout << "1. Изменить роторы\n";
        std::cout << "2. Изменить значения на кольцах\n";
        std::cout << "3. Изменить рефлектор\n";
        std::cout << "0. Вернуться в предыдущее меню\n";
        std::cout << "Ваш выбор: ";
        
        int choice;
        if (!get_valid_integer(choice, 0, 3)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 3.\n";
            std::cout << "Попробуйте снова.\n";
            continue;
        }
        
        if (choice == 0) {
            return;
        }
        
        switch (choice) {
            case 1: {
                configure_rotors_only();
                return;
            }
            case 2: {
                std::string rings;
                size_t rotor_count = machine.get_rotor_count();
                
                input_rings(rotor_count, rings);
                try {
                    machine.change_ring_positions(rings);
                    std::cout << "Позиции колец изменены!\n";
                    machine.display_configuration();
                    return;
                } catch (const std::exception& e) {
                    std::cout << "Ошибка при изменении позиций колец: " << e.what() << "\n\n";
                    return;
                }
            }
            case 3: {
                char reflector_type;
                input_reflector(reflector_type);
                try {
                    machine.setup_reflector(reflector_type);
                    std::cout << "Рефлектор изменен!\n";
                    machine.display_configuration();
                    return;
                } catch (const std::exception& e) {
                    std::cout << "Ошибка при изменении рефлектора: " << e.what() << "\n\n";
                    return;
                }
            }
        }
    }
}

void ConsoleInterface::modify_existing_machine() {
    while (true) {
        std::cout << "\nТекущая конфигурация машины:\n";
        machine.display_configuration();
        
        std::cout << "Выберите тип изменения:\n";
        std::cout << "1. Настроить заново (полная перенастройка)\n";
        std::cout << "2. Модифицировать частично (изменить отдельные компоненты)\n";
        std::cout << "0. Вернуться в главное меню\n";
        std::cout << "Ваш выбор: ";
        
        int choice;
        if (!get_valid_integer(choice, 0, 2)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 2.\n";
            std::cout << "Попробуйте снова.\n";
            continue;
        }
        
        if (choice == 0) {
            return;
        }
        
        switch (choice) {
            case 1: {
                configure_new_machine();
                return;
            }
            case 2: {
                modify_existing_machine_partial();
                return;
            }
        }
    }
}

bool ConsoleInterface::input_rotors(std::vector<uint8_t>& rotors) {
    size_t max_rotors = Enigma::get_available_rotors_count();
    
    while (true) {
        rotors.clear();
        
        std::cout << "Введите номера роторов установки в порядке следования к рефлектору (1-" 
                  << max_rotors << ", через пробел): ";
        std::string line;
        std::getline(std::cin, line);
        
        std::istringstream iss(line);
        std::vector<int> temp_rotors;
        int rotor_num;
        bool valid_input = true;
        
        while (iss >> rotor_num) {
            if (rotor_num < 1 || rotor_num > static_cast<int>(max_rotors)) {
                std::cout << "Ошибка: Неверный номер ротора (" << rotor_num 
                          << "). Должен быть от 1 до " << max_rotors << ".\n";
                valid_input = false;
                break;
            }
            
            if (std::find(temp_rotors.begin(), temp_rotors.end(), rotor_num) != temp_rotors.end()) {
                std::cout << "Ошибка: Ротор " << rotor_num << " уже используется. Роторы не могут повторяться.\n";
                valid_input = false;
                break;
            }
            
            temp_rotors.push_back(rotor_num);
        }
        
        if (valid_input && (temp_rotors.empty() || temp_rotors.size() > max_rotors)) {
            std::cout << "Ошибка: Количество роторов должно быть от 1 до " << max_rotors << ".\n";
            valid_input = false;
        }
        
        if (valid_input) {
            for (int num : temp_rotors) {
                rotors.push_back(static_cast<uint8_t>(num));
            }
            return true;
        }
        
        std::cout << "Попробуйте снова.\n";
    }
}

bool ConsoleInterface::input_rings(const size_t rotor_count, std::string& rings) {
    rings.clear();
    rings.reserve(rotor_count);
    
    std::cout << "Введите позиции колец для " << rotor_count << " ротора(-ов) (A-Z, через пробел): ";
    
    for (size_t i = 0; i < rotor_count; ++i) {
        char ring;
        std::cin >> ring;
        
        ring = std::toupper(ring);
        if (ring < 'A' || ring > 'Z') {
            std::cout << "Неверная позиция кольца: " << ring << ". Должна быть буква от A до Z.\n\n";
            clear_input_buffer();
            return false;
        }
        
        rings += ring;
    }
    
    clear_input_buffer();
    return true;
}

bool ConsoleInterface::input_reflector(char& reflector_type) {
    const auto& reflector_configs = Enigma::get_reflector_configs();
    
    while (true) {
        std::cout << "Введите тип рефлектора (";
        
        std::vector<char> available_reflectors;
        for (const auto& pair : reflector_configs) {
            available_reflectors.push_back(pair.first);
        }
        
        std::sort(available_reflectors.begin(), available_reflectors.end());
        
        for (size_t i = 0; i < available_reflectors.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << available_reflectors[i];
        }
        std::cout << "): ";
        
        std::cin >> reflector_type;
        clear_input_buffer();
        
        reflector_type = std::toupper(reflector_type);
        
        if (reflector_configs.find(reflector_type) != reflector_configs.end()) {
            return true;
        }
        
        std::cout << "Ошибка: Неверный тип рефлектора '" << reflector_type << "'. Должен быть один из: ";
        for (size_t i = 0; i < available_reflectors.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << available_reflectors[i];
        }
        std::cout << ".\n";
        std::cout << "Попробуйте снова.\n";
    }
}

void ConsoleInterface::clear_input_buffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool ConsoleInterface::get_valid_integer(int& value, int min_val, int max_val) {
    if (!(std::cin >> value)) {
        std::cin.clear();
        clear_input_buffer();
        return false;
    }
    
    if (value < min_val || value > max_val) {
        clear_input_buffer();
        return false;
    }
    
    clear_input_buffer();
    return true;
}