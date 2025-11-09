CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = enigma
SOURCES = main.cpp console_interface.cpp enigma.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = console_interface.h enigma.h exceptions.h

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean $(TARGET)

.PHONY: clean rebuild

# Создание тестовых архивов для демонстрации в папке out/
test_archives:
	@echo "Создание директории out/..."
	@mkdir -p out
	@echo "Создание тестовых файлов..."
	@echo "Это тестовый файл для архивации" > test1.txt
	@echo "Еще один тестовый файл" > test2.txt
	@echo "Третий файл с данными для тестирования" > test3.txt
	@if command -v zip >/dev/null 2>&1; then \
		zip out/test_archive.zip test1.txt test2.txt test3.txt; \
		echo "Создан out/test_archive.zip"; \
	else \
		echo "zip не найден, пропускаем создание .zip архива"; \
	fi
	@if command -v tar >/dev/null 2>&1; then \
		tar -czf out/test_archive.tar.gz test1.txt test2.txt test3.txt; \
		echo "Создан out/test_archive.tar.gz"; \
	else \
		echo "tar не найден, пропускаем создание .tar.gz архива"; \
	fi
	@rm -f test1.txt test2.txt test3.txt
	@echo "Тестовые архивы готовы для шифрования в папке out/!"

# Очистка всех файлов в папке out/
clean_out:
	@echo "Очистка папки out/..."
	@rm -rf out/*
	@echo "Папка out/ очищена!"

.PHONY: test_archives clean_out