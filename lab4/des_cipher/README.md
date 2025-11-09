# DES Cipher - Лабораторная работа 2.2

## Структура проекта:
- `main.cpp` - точка входа
- `des.h/cpp` - реализация алгоритма DES
- `des_tables.h` - таблицы перестановок и S-блоки
- `console_interface.h/cpp` - консольный интерфейс
- `exceptions.h` - обработка исключений
- `out/` - директория для входных/выходных файлов

## Сборка:
```bash
make
```

## Запуск:
```bash
./des_cipher
```

## Очистка:
```bash
make clean
```

## Тестирование с пустым файлом:
```bash
touch out/empty.txt
echo "X" > out/single.txt
./des_cipher
```
