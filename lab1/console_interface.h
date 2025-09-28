#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>

#include "enigma.h"

class ConsoleInterface {
public:
    ConsoleInterface();
    void run();

private:
    Enigma machine;
    
    void display_main_menu();
    void handle_encrypt_message();
    void handle_change_configuration();
    void handle_display_configuration();
    void handle_reset_machine();
    
    void display_available_rotors();
    void configure_new_machine();
    void modify_existing_machine();
    void modify_existing_machine_partial();
    
    bool input_rotors(std::vector<uint8_t>& rotors);
    bool input_rings(const size_t rotor_count, std::string& rings);
    bool input_reflector(char& reflector_type);
    void configure_rotors_only();
    
    void handle_message_input_choice();
    std::string read_message_from_keyboard();
    std::string read_message_from_file();
    void handle_output_choice(const std::string& encrypted_message);
    void write_message_to_file(const std::string& message);
    bool create_output_directory();
    std::vector<std::string> get_txt_files_in_out_directory();
    void display_available_files(const std::vector<std::string>& files);
    
    void clear_input_buffer();
    bool get_valid_integer(int& value, int min_val, int max_val);
};