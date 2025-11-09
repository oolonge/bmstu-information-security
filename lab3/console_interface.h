#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <filesystem>
#include <algorithm>
#include "rsa.h"

class ConsoleInterface {
public:
    ConsoleInterface();
    void run();

private:
    RSA rsa;
    
    // Отображение меню
    void display_main_menu();
    
    // Обработчики пунктов меню
    void handle_generate_keys();
    void handle_load_keys();
    void handle_save_keys();
    void handle_encrypt_file();
    void handle_decrypt_file();
    void handle_display_keys();
    
    // Вспомогательные методы для работы с файлами
    bool create_output_directory();
    std::vector<std::string> get_files_in_out_directory(const std::string& extension);
    void display_available_files(const std::vector<std::string>& files);
    std::string select_file_from_list(const std::vector<std::string>& files, const std::string& prompt);
    
    // Вспомогательные методы для ввода
    void clear_input_buffer();
    bool get_valid_integer(int& value, int min_val, int max_val);
    std::string get_filename_input(const std::string& prompt);
};