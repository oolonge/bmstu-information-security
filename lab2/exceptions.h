#include <stdexcept>
#include <string>

namespace EnigmaExceptions { 
    static const size_t MAX_ROTORS = 5;

    class InvalidState : public std::logic_error {
    public: 
        InvalidState(std::string str) : std::logic_error(str) {}
    };

    class InvalidRotor : public std::invalid_argument {
    public:
        InvalidRotor(uint8_t num) : std::invalid_argument("Несуществующий ротор: " + std::to_string(num)) {}
    };

    class RotorsWrongNumber : public std::length_error {
    public:
        RotorsWrongNumber(size_t count) : std::length_error("неправильное число роторов: " + std::to_string(count)) {}
    };

    class DuplicateRotor : public std::logic_error {
    public: 
        DuplicateRotor(uint8_t num) : std::logic_error("Дублирующийся ротор: " + std::to_string(num) + "; Роторы не могут повторятся") {}
    };

    class NotALetter : public std::logic_error { 
    public: 
        NotALetter(char c) : std::logic_error("Не подходящая буква для кольца: " + std::string(1, c)) {}
    };

    class RingMissmatch : public std::logic_error {
    public:
        RingMissmatch(uint8_t recieved, uint8_t needed) : std::logic_error("Количество колец не соответствует количеству роторов: "\
     + std::to_string(recieved) + "/" + std::to_string(needed)) {}
    };

    class WrongRelectorType : public std::logic_error {
    public:
        WrongRelectorType(char c) : std::logic_error("Выбран несуществующий тип рефлектора: " + std::string(1, c)) {}
    };

    class MachineUninitialized : public std::logic_error { 
    public:
        MachineUninitialized() : std::logic_error("Энигма ещё не настроена") {}
    };

    class InvalidCharacter : public std::logic_error {
    public: 
    InvalidCharacter(char c) : std::logic_error("Обработка символа (" + std::string(1, c) + ") не предусмотрена машиной") {}
    };

}