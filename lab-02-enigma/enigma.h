#pragma once
#include <vector> 
#include <string> 
#include <unordered_map>

class Enigma
{
public: 
    Enigma() : state(SetupState::Empty), is_intitialized(false) {}

    void setup(const std::vector<uint8_t>& rotors, 
        const std::string& rings, 
        char reflector_type);
    void setup_rotors(const std::vector<uint8_t>& rotors);
    void setup_rings(const std::string& rings);
    void setup_reflector(const char& c);
    
    std::string encrypt_message(const std::string& message);
    std::vector<uint8_t> encrypt_binary_data(const std::vector<uint8_t>& data);

    void reset_to_initial_positions();

    void display_configuration() const;
    bool is_initialized() const { return is_intitialized; }
    size_t get_rotor_count() const { return active_rotors.size(); }
    std::vector<char> get_current_positions() const { return rotor_positions; }
    void change_ring_positions(const std::string& rings);
    
    void setup_rotors_only(const std::vector<uint8_t>& rotors);

    static size_t get_available_rotors_count();
    static size_t get_available_reflectors_count();
    static const std::unordered_map<uint8_t, std::pair<std::vector<uint8_t>, uint8_t>>& get_rotor_configs_extended();
    static const std::unordered_map<char, std::vector<uint8_t>>& get_reflector_configs_extended();
    // Старые методы для совместимости с текстом
    static const std::unordered_map<uint8_t, std::pair<std::string, char>>& get_rotor_configs();
    static const std::unordered_map<char, std::string>& get_reflector_configs();

private:
    // Старые конфигурации для текста (A-Z)
    static const std::unordered_map<uint8_t, std::pair<std::string, char>> ROTOR_CONFIGS;
    static const std::unordered_map<char, std::string> REFLECTOR_CONFIGS;
    
    // Новые конфигурации для бинарных данных (0-255)
    static const std::unordered_map<uint8_t, std::pair<std::vector<uint8_t>, uint8_t>> ROTOR_CONFIGS_EXTENDED;
    static const std::unordered_map<char, std::vector<uint8_t>> REFLECTOR_CONFIGS_EXTENDED;

    void step_rotors();
    void step_rotors_extended();
    void apply_rotor(const std::pair<std::string, char>& rotor, char& input);
    void apply_rotor_inverse(const std::pair<std::string, char>& rotor, char& input);
    void apply_rotor_extended(const std::pair<std::vector<uint8_t>, uint8_t>& rotor, uint8_t& input);
    void apply_rotor_inverse_extended(const std::pair<std::vector<uint8_t>, uint8_t>& rotor, uint8_t& input);

    char encrypt_letter(char input);
    uint8_t encrypt_byte(uint8_t input);

    enum class SetupState {
        Empty,
        RotorsConfigured,
        FullyConfigured
    } state;

    std::vector<std::pair<std::string, char>> active_rotors;
    std::vector<std::pair<std::vector<uint8_t>, uint8_t>> active_rotors_extended;
    std::vector<char> initial_rotor_positions;
    std::vector<char> rotor_positions;
    std::vector<uint8_t> initial_rotor_positions_extended;
    std::vector<uint8_t> rotor_positions_extended;
    std::string reflector;   
    std::vector<uint8_t> reflector_extended;
    bool is_intitialized = false; 
};