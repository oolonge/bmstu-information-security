#include <cctype>
#include <iostream> 

#include "exceptions.h"
#include "console_interface.h"

int main()
{
    try {
        ConsoleInterface interface;
        interface.run();
    } catch (const std::exception& e) {
        std::cerr << "\nНепредвиденная ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}