#include <cctype>
#include <iostream> 
#include <vector> 
#include <string> 
#include <unordered_map>
#include <algorithm>

#include "enigma.h"
#include "exceptions.h"

const std::unordered_map<uint8_t, std::pair<std::string, char>> Enigma::ROTOR_CONFIGS = {{  // <type, <letters_shift, notch>>
    {1, {"EKMFLGDQVZNTOWYHXUSPAIBRCJ", 'A'}},  
    {2, {"AJDKSIRUXBLHWTMCQGZNPYFVOE", 'B'}}, 
    {3, {"BDFHJLCPRTXVZNYEIWGAKMUSQO", 'C'}}, 
    {4, {"ESOVPZJAYQUIRHXLNFTGKDCMWB", 'R'}},
    {5, {"VZBRGITYUPSDNHLXAWMJQOFECK", 'H'}}
}};

const std::unordered_map<char, std::string> Enigma::REFLECTOR_CONFIGS = {{
    {'A', "EJMZALYXVBWFCRQUONTSPIKHGD"},
    {'B', "YRUHQSLDPXNGOKMIEBFZCWVJAT"},
    {'C', "FVPJIAOYEDRZXWGCTKUQSBNMHL"}
}};

size_t Enigma::get_available_rotors_count() {
    return ROTOR_CONFIGS.size();
}

size_t Enigma::get_available_reflectors_count() {
    return REFLECTOR_CONFIGS.size();
}

const std::unordered_map<uint8_t, std::pair<std::string, char>>& Enigma::get_rotor_configs() {
    return ROTOR_CONFIGS;
}

const std::unordered_map<char, std::string>& Enigma::get_reflector_configs() {
    return REFLECTOR_CONFIGS;
}

void Enigma::setup_rotors(const std::vector<uint8_t>& rotors)
{
    if (state != SetupState::Empty) { throw EnigmaExceptions::InvalidState("Роторы уже настроены"); }
    if (rotors.size() > ROTOR_CONFIGS.size() || rotors.size() < 1) { 
        throw EnigmaExceptions::RotorsWrongNumber(rotors.size()); 
    }
    std::vector<uint8_t> used_rotors;

    for (uint8_t num : rotors) {
        if (ROTOR_CONFIGS.find(num) == ROTOR_CONFIGS.end()) { 
            throw EnigmaExceptions::InvalidRotor(num); 
        }
        if (std::find(used_rotors.begin(), used_rotors.end(), num) != used_rotors.end()) {
            throw EnigmaExceptions::DuplicateRotor(num);
        }
        used_rotors.push_back(num);
        active_rotors.push_back(ROTOR_CONFIGS.at(num));
    }
    state = SetupState::RotorsConfigured;
}

void Enigma::setup_rotors_only(const std::vector<uint8_t>& rotors)
{
    if (!is_intitialized) { 
        throw EnigmaExceptions::InvalidState("Машина не настроена"); 
    }
    if (rotors.size() > ROTOR_CONFIGS.size() || rotors.size() < 1) { 
        throw EnigmaExceptions::RotorsWrongNumber(rotors.size()); 
    }
    
    std::vector<uint8_t> used_rotors;
    std::vector<std::pair<std::string, char>> new_rotors;

    for (uint8_t num : rotors) {
        if (ROTOR_CONFIGS.find(num) == ROTOR_CONFIGS.end()) { 
            throw EnigmaExceptions::InvalidRotor(num); 
        }
        if (std::find(used_rotors.begin(), used_rotors.end(), num) != used_rotors.end()) {
            throw EnigmaExceptions::DuplicateRotor(num);
        }
        used_rotors.push_back(num);
        new_rotors.push_back(ROTOR_CONFIGS.at(num));
    }
    
    std::vector<char> old_positions = rotor_positions;
    std::vector<char> old_initial_positions = initial_rotor_positions;
    
    active_rotors = new_rotors;
    
    if (old_positions.size() != active_rotors.size()) {
        rotor_positions.clear();
        initial_rotor_positions.clear();
        for (size_t i = 0; i < active_rotors.size(); ++i) {
            rotor_positions.push_back('A');
            initial_rotor_positions.push_back('A');
        }
    } else {
        rotor_positions = old_positions;
        initial_rotor_positions = old_initial_positions;
    }
}

void Enigma::setup_reflector(const char& c) {
    if (REFLECTOR_CONFIGS.find(c) == REFLECTOR_CONFIGS.end()) { 
        throw EnigmaExceptions::WrongRelectorType(c); 
    }
    reflector = REFLECTOR_CONFIGS.at(c);
}

void Enigma::display_configuration() const {
    if (!is_intitialized) {
        std::cout << "Машина ещё не настроена. Сначала настройте конфигурацию\n\n";
        return;
    }
    
    std::cout << "=== Текущая конфигурация ===\n";
    std::cout << "Роторы:\n";
    for (size_t i = 0; i < active_rotors.size(); ++i) {
        std::cout << "  Позиция " << (i + 1) << ": ";
        std::cout << "Барабан: " << active_rotors[i].first;
        std::cout << " | Notch: " << active_rotors[i].second;
        std::cout << " | Ring: " << rotor_positions[i] << "\n";
    }
    
    std::cout << "  Рефлектор: Барабан: " << reflector << " | Тип: ";
    for (const auto& pair : REFLECTOR_CONFIGS) {
        if (pair.second == reflector) {
            std::cout << pair.first;
            break;
        }
    }
    std::cout << "\n\n";
}



void Enigma::setup_rings(const std::string& rings)
{
    if (state != SetupState::RotorsConfigured) { throw EnigmaExceptions::InvalidState("Сначала нужно настроить роторы"); }
    if (active_rotors.size() != rings.length()) { throw EnigmaExceptions::RingMissmatch(rings.length(), active_rotors.size()); }

    for (char c : rings) {
        if (c < 'A' || c > 'Z') { throw EnigmaExceptions::NotALetter(c); }
        rotor_positions.push_back(c);
    }
    initial_rotor_positions = rotor_positions; 
    state = SetupState::FullyConfigured;
}


void Enigma::setup(const std::vector<uint8_t>& rotors, 
    const std::string& rings, 
    char reflector_type)
{
    setup_rotors(rotors);
    setup_rings(rings);
    setup_reflector(reflector_type);
    is_intitialized = true;
}

char shift_letters(const char c, const int shift)
{
    int char_index = c - 'A';
    int shifted_index = ((char_index + shift) % 26 + 26) % 26;
    return static_cast<char>(shifted_index + 'A');
}

void Enigma::step_rotors()
{
    size_t rotors_number = active_rotors.size();
    std::vector<int> shifts(rotors_number, 0);
    shifts[0] = 1;
    
    for (size_t i = 0; i < rotors_number - 1; ++i) {
        if (active_rotors[i].second == rotor_positions[i]) {
            shifts[i + 1] = 1;
        }
    }
    
    for (size_t i = 1; i < rotors_number - 1; ++i) {
        if (active_rotors[i].second == rotor_positions[i]) {
            shifts[i] = 1;
        }
    }
    
    for (size_t i = 0; i < rotors_number; ++i) {
        if (shifts[i] > 0) {
            rotor_positions[i] = shift_letters(rotor_positions[i], shifts[i]);
        }
    }
}

void Enigma::apply_rotor(const std::pair<std::string, char>& rotor, char& input)
{
    input = rotor.first[input - 'A'];
}

void Enigma::apply_rotor_inverse(const std::pair<std::string, char>& rotor, char& input)
{
    for (int i = 0; i < 26; ++i) {
        if (rotor.first[i] == input) {
            input = static_cast<char>('A' + i);
            return;
        }
    }
}

char Enigma::encrypt_letter(char input)
{
    char current_letter = input;

    current_letter = shift_letters(current_letter, rotor_positions[0] - 'A');
    apply_rotor(active_rotors[0], current_letter);

    for (int i = 1; i < rotor_positions.size(); ++i) {
        current_letter = shift_letters(current_letter, rotor_positions[i] - rotor_positions[i - 1]);
        apply_rotor(active_rotors[i], current_letter);
    }

    current_letter = reflector[current_letter - 'A'];
    apply_rotor_inverse(active_rotors.back(), current_letter);
    
    for (int i = active_rotors.size() - 2; i >= 0; --i) {
        current_letter = shift_letters(current_letter,  rotor_positions[i] - rotor_positions[i + 1]);  // -(rotor_positions[i + 1] - rotor_positions[i])
        apply_rotor_inverse(active_rotors[i], current_letter);
    }

    return shift_letters(current_letter, 'A' - rotor_positions[0]); 
}

std::string Enigma::encrypt_message(const std::string& message)
{
    if (!is_intitialized) { throw EnigmaExceptions::MachineUninitialized(); }

    std::string encrypted_message;
    encrypted_message.reserve(message.length());

    for (const char& c : message)
    {
        char letter = c;
        if (letter >= 'a' && letter <= 'z') {
            letter = std::toupper(letter);
        } else if (!(letter >= 'A' && letter <= 'Z')) {
            throw EnigmaExceptions::InvalidCharacter(c);
        }
        
        step_rotors();
        encrypted_message += encrypt_letter(letter);
    }

    return encrypted_message;
}

void Enigma::reset_to_initial_positions()
{
    if (is_intitialized && !initial_rotor_positions.empty()) {
        rotor_positions = initial_rotor_positions;
    }
}

void Enigma::change_ring_positions(const std::string& rings)
{
    if (!is_intitialized) { 
        throw EnigmaExceptions::InvalidState("Машина не настроена"); 
    }
    if (active_rotors.size() != rings.length()) { 
        throw EnigmaExceptions::RingMissmatch(rings.length(), active_rotors.size()); 
    }

    rotor_positions.clear();
    for (char c : rings) {
        if (c < 'A' || c > 'Z') { 
            throw EnigmaExceptions::NotALetter(c); 
        }
        rotor_positions.push_back(c);
    }
    initial_rotor_positions = rotor_positions; 
}