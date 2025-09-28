CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = enigma
SOURCES = main.cpp console_interface.cpp enigma.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h enigma.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild