#include "console_interface.h"
#include "exceptions.h"
#include <sstream>

ConsoleInterface::ConsoleInterface() {}

void ConsoleInterface::run() {
    std::cout << "=== RSA Шифрование - Лабораторная работа 2 ===\n\n";
    
    while (true) {
        display_main_menu();
        
        int choice;
        std::cout << "> ";
        
        if (!get_valid_integer(choice, 0, 6)) {
            std::cout << "Ошибка: Неверный ввод. Введите число от 0 до 6.\n";
            std::cout << "Попробуйте снова.\n\n";
            continue;
        }
        
        if (choice == 0) {
            std::cout << "Выход из программы.\n";
            break;
        }
        
        try {
            switch (choice) {
                case 1:
                    handle_generate_keys();
                    break;
                case 2:
                    handle_load_keys();
                    break;
                case 3:
                    handle_save_keys();
                    break;
                case 4:
                    handle_encrypt_file();
                    break;
                case 5:
                    handle_decrypt_file();
                    break;
                case 6:
                    handle_display_keys();
                    break;
                default:
                    std::cout << "Неверный выбор. Попробуйте снова.\n\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << "\n\n";
        }
    }
}

void ConsoleInterface::display_main_menu() {
    std::cout << "=== RSA Шифрование ===\n";
    std::cout << "1. Сгенерировать ключи (512 бит)\n";
    std::cout << "2. Загрузить ключи из файлов\n";
    std::cout << "3. Сохранить ключи в файлы\n";
    std::cout << "4. Зашифровать файл\n";
    std::cout << "5. Расшифровать файл\n";
    std::cout << "6. Показать публичный ключ\n";
    std::cout << "0. Выход\n";
}

void ConsoleInterface::handle_generate_keys() {
    std::cout << "\n=== Генерация ключей ===\n";
    std::cout << "Это может занять некоторое время...\n\n";
    
    rsa.generate_keys(512);
    
    std::cout << "\n";
}

void ConsoleInterface::handle_load_keys() {
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать или получить доступ к директории out/");
    }
    
    std::cout << "\n=== Загрузка ключей ===\n";
    
    std::string public_key_file = get_filename_input("Введите имя файла публичного ключа (без пути, в директории out/): ");
    std::string private_key_file = get_filename_input("Введите имя файла приватного ключа (без пути, в директории out/): ");
    
    std::string public_path = "out/" + public_key_file;
    std::string private_path = "out/" + private_key_file;
    
    rsa.load_keys(public_path, private_path);
    
    std::cout << "\n";
}

void ConsoleInterface::handle_save_keys() {
    if (!rsa.is_initialized()) {
        std::cout << "Ключи еще не сгенерированы. Сначала создайте ключи (пункт 1).\n\n";
        return;
    }
    
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать директорию out/");
    }
    
    std::cout << "\n=== Сохранение ключей ===\n";
    
    std::string public_key_file = get_filename_input("Введите имя файла для публичного ключа (без расширения): ");
    std::string private_key_file = get_filename_input("Введите имя файла для приватного ключа (без расширения): ");
    
    if (public_key_file.empty()) public_key_file = "public";
    if (private_key_file.empty()) private_key_file = "private";
    
    std::string public_path = "out/" + public_key_file + ".key";
    std::string private_path = "out/" + private_key_file + ".key";
    
    rsa.save_keys(public_path, private_path);
    
    std::cout << "\n";
}

void ConsoleInterface::handle_encrypt_file() {
    if (!rsa.is_initialized()) {
        std::cout << "Ключи еще не сгенерированы. Сначала создайте или загрузите ключи.\n\n";
        return;
    }
    
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать или получить доступ к директории out/");
    }
    
    std::cout << "\n=== Шифрование файла ===\n";
    
    // Получаем список всех файлов (кроме .enc и .key)
    std::vector<std::string> all_files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator("out/")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();
                
                // Пропускаем зашифрованные файлы и файлы ключей
                if (ext != ".enc" && ext != ".key") {
                    all_files.push_back(filename);
                }
            }
        }
        std::sort(all_files.begin(), all_files.end());
    } catch (const std::exception&) {
        // Игнорируем ошибки при чтении директории
    }
    
    if (all_files.empty()) {
        std::cout << "В директории out/ не найдено файлов для шифрования.\n";
        std::cout << "Поместите файлы в директорию out/ и попробуйте снова.\n\n";
        return;
    }
    
    std::string selected_file = select_file_from_list(all_files, "Выберите файл для шифрования");
    
    if (selected_file.empty()) {
        return;
    }
    
    std::string input_path = "out/" + selected_file;
    
    // Генерируем имя выходного файла
    std::string output_file = selected_file + ".enc";
    std::string output_path = "out/" + output_file;
    
    std::cout << "\nШифрование: " << selected_file << " -> " << output_file << "\n";
    
    rsa.encrypt_file(input_path, output_path);
    
    std::cout << "Зашифрованный файл сохранен: " << output_path << "\n\n";
}

void ConsoleInterface::handle_decrypt_file() {
    if (!rsa.is_initialized()) {
        std::cout << "Ключи еще не сгенерированы. Сначала создайте или загрузите ключи.\n\n";
        return;
    }
    
    if (!create_output_directory()) {
        throw std::runtime_error("Не удалось создать или получить доступ к директории out/");
    }
    
    std::cout << "\n=== Расшифровка файла ===\n";
    
    std::vector<std::string> enc_files = get_files_in_out_directory(".enc");
    
    if (enc_files.empty()) {
        std::cout << "В директории out/ не найдено зашифрованных файлов (.enc).\n\n";
        return;
    }
    
    std::string selected_file = select_file_from_list(enc_files, "Выберите файл для расшифровки");
    
    if (selected_file.empty()) {
        return;
    }
    
    std::string input_path = "out/" + selected_file;
    
    // Удаляем расширение .enc из имени
    std::string output_file = selected_file;
    if (output_file.size() > 4 && output_file.substr(output_file.size() - 4) == ".enc") {
        output_file = output_file.substr(0, output_file.size() - 4);
    }
    output_file = "decrypted_" + output_file;
    
    std::string output_path = "out/" + output_file;
    
    std::cout << "\nРасшифровка: " << selected_file << " -> " << output_file << "\n";
    
    rsa.decrypt_file(input_path, output_path);
    
    std::cout << "Расшифрованный файл сохранен: " << output_path << "\n\n";
}

void ConsoleInterface::handle_display_keys() {
    if (!rsa.is_initialized()) {
        std::cout << "Ключи еще не сгенерированы. Сначала создайте или загрузите ключи.\n\n";
        return;
    }
    
    rsa.display_public_key();
}

bool ConsoleInterface::create_output_directory() {
    try {
        std::filesystem::create_directories("out");
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> ConsoleInterface::get_files_in_out_directory(const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        if (!std::filesystem::exists("out/")) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator("out/")) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                files.push_back(entry.path().filename().string());
            }
        }
        
        std::sort(files.begin(), files.end());
        
    } catch (const std::exception&) {
        files.clear();
    }
    
    return files;
}

void ConsoleInterface::display_available_files(const std::vector<std::string>& files) {
    std::cout << "\nДоступные файлы:\n";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << files[i] << "\n";
    }
    std::cout << "\n";
}

std::string ConsoleInterface::select_file_from_list(const std::vector<std::string>& files, const std::string& prompt) {
    display_available_files(files);
    
    std::cout << prompt << " (введите номер от 1 до " << files.size() << ", 0 для отмены): ";
    
    int choice;
    if (!get_valid_integer(choice, 0, static_cast<int>(files.size()))) {
        std::cout << "Неверный выбор.\n\n";
        return "";
    }
    
    if (choice == 0) {
        std::cout << "Отмена.\n\n";
        return "";
    }
    
    return files[choice - 1];
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

std::string ConsoleInterface::get_filename_input(const std::string& prompt) {
    std::string filename;
    std::cout << prompt;
    std::getline(std::cin, filename);
    
    // Очистка имени файла от недопустимых символов
    std::string cleaned;
    for (char c : filename) {
        if (std::isalnum(c) || c == '_' || c == '-' || c == '.') {
            cleaned += c;
        } else if (c == ' ') {
            cleaned += '_';
        }
    }
    
    return cleaned;
}