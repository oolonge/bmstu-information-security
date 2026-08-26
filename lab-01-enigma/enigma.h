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

    void reset_to_initial_positions();

    void display_configuration() const;
    bool is_initialized() const { return is_intitialized; }
    size_t get_rotor_count() const { return active_rotors.size(); }
    std::vector<char> get_current_positions() const { return rotor_positions; }
    void change_ring_positions(const std::string& rings);
    
    void setup_rotors_only(const std::vector<uint8_t>& rotors);

    static size_t get_available_rotors_count();
    static size_t get_available_reflectors_count();
    static const std::unordered_map<uint8_t, std::pair<std::string, char>>& get_rotor_configs();
    static const std::unordered_map<char, std::string>& get_reflector_configs();

private:
    static const std::unordered_map<uint8_t, std::pair<std::string, char>> ROTOR_CONFIGS;
    static const std::unordered_map<char, std::string> REFLECTOR_CONFIGS;

    void step_rotors();
    void apply_rotor(const std::pair<std::string, char>& rotor, char& input);
    void apply_rotor_inverse(const std::pair<std::string, char>& rotor, char& input);

    char encrypt_letter(char input);

    enum class SetupState {
        Empty,
        RotorsConfigured,
        FullyConfigured
    } state;

    std::vector<std::pair<std::string, char>> active_rotors;
    std::vector<char> initial_rotor_positions;
    std::vector<char> rotor_positions;
    std::string reflector;   
    bool is_intitialized = false; 
};