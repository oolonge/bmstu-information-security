#!/bin/bash

# Создание директории проекта
mkdir -p rsa_cipher
cd rsa_cipher

# Создание директории для файлов
mkdir -p out

# Создание заголовочных файлов
touch rsa.h
touch console_interface.h
touch exceptions.h

# Создание файлов реализации
touch rsa.cpp
touch console_interface.cpp
touch main.cpp

# Создание Makefile
touch Makefile

# Создание README для справки
cat > README.md << 'EOF'
# RSA Cipher - Лабораторная работа 2

## Структура проекта:
- `main.cpp` - точка входа
- `rsa.h/cpp` - реализация алгоритма RSA
- `console_interface.h/cpp` - консольный интерфейс
- `exceptions.h` - обработка исключений
- `out/` - директория для входных/выходных файлов

## Сборка:
```bash
make
