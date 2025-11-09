CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I/opt/homebrew/opt/gmp/include
LDFLAGS = -L/opt/homebrew/opt/gmp/lib -lgmp -lgmpxx
TARGET = rsa_cipher
SOURCES = main.cpp console_interface.cpp rsa.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h rsa.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild