#include <iostream>
#include "console_interface.h"
#include "exceptions.h"

int main() {
    try {
        ConsoleInterface interface;
        interface.run();
    } catch (const std::exception& e) {
        std::cerr << "\nНепредвиденная ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
