CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = rsa_signature
SOURCES = main.cpp console_interface.cpp rsa.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h rsa.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild
