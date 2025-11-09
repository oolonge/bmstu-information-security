CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = des_cipher
SOURCES = main.cpp console_interface.cpp des.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h des.h des_tables.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild