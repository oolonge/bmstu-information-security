CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = aes_cipher
SOURCES = main.cpp console_interface.cpp aes.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h aes.h aes_tables.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild