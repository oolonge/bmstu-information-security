#include "aes.h"
#include "aes_tables.h"
#include "exceptions.h"
#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include <cstring>

AES::AES() : key_generated(false) {
    std::memset(key, 0, sizeof(key));
    std::memset(round_keys, 0, sizeof(round_keys));
}

void AES::generate_key() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    for (int i = 0; i < 32; ++i) {
        key[i] = dist(gen);
    }
    
    key_expansion();
    key_generated = true;
    
    std::cout << "Ключ успешно сгенерирован!\n";
}

void AES::save_key(const std::string& path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw DESExceptions::FileWriteError(path);
    }
    
    file.write(reinterpret_cast<const char*>(key), sizeof(key));
    file.close();
    
    std::cout << "Ключ сохранен: " << path << "\n";
}

void AES::load_key(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw DESExceptions::InvalidKeyFile(path);
    }
    
    file.read(reinterpret_cast<char*>(key), sizeof(key));
    
    if (!file) {
        throw DESExceptions::InvalidKeyFile(path);
    }
    
    file.close();
    
    key_expansion();
    key_generated = true;
    
    std::cout << "Ключ успешно загружен\n";
}

void AES::display_key() const {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::cout << "\n=== AES Ключ ===\n";
    std::cout << "Ключ (hex): ";
    for (int i = 0; i < 32; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(key[i]);
    }
    std::cout << std::dec << "\n\n";
}

uint32_t AES::sub_word(uint32_t word) {
    uint32_t result = 0;
    result |= AESTable::S_BOX[(word >> 24) & 0xFF] << 24;
    result |= AESTable::S_BOX[(word >> 16) & 0xFF] << 16;
    result |= AESTable::S_BOX[(word >> 8) & 0xFF] << 8;
    result |= AESTable::S_BOX[word & 0xFF];
    return result;
}

uint32_t AES::rot_word(uint32_t word) {
    return (word << 8) | (word >> 24);
}

void AES::key_expansion() {
    int nk = 8;  // Количество 32-битных слов в ключе для AES-256
    int nr = 14; // Количество раундов для AES-256
    
    // Копируем исходный ключ в первые nk слов
    for (int i = 0; i < nk; ++i) {
        round_keys[i] = (key[4*i] << 24) | (key[4*i+1] << 16) | 
                        (key[4*i+2] << 8) | key[4*i+3];
    }
    
    // Генерируем остальные раундовые ключи
    for (int i = nk; i < 4 * (nr + 1); ++i) {
        uint32_t temp = round_keys[i - 1];
        
        if (i % nk == 0) {
            temp = sub_word(rot_word(temp)) ^ (AESTable::RCON[i/nk - 1] << 24);
        } else if (i % nk == 4) {
            temp = sub_word(temp);
        }
        
        round_keys[i] = round_keys[i - nk] ^ temp;
    }
}

void AES::bytes_to_state(const uint8_t* input, uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[j][i] = input[i * 4 + j];
        }
    }
}

void AES::state_to_bytes(const uint8_t state[4][4], uint8_t* output) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            output[i * 4 + j] = state[j][i];
        }
    }
}

void AES::sub_bytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = AESTable::S_BOX[state[i][j]];
        }
    }
}

void AES::inv_sub_bytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = AESTable::INV_S_BOX[state[i][j]];
        }
    }
}

void AES::shift_rows(uint8_t state[4][4]) {
    uint8_t temp;
    
    // Вторая строка: сдвиг на 1 влево
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;
    
    // Третья строка: сдвиг на 2 влево
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    
    // Четвертая строка: сдвиг на 3 влево (или 1 вправо)
    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;
}

void AES::inv_shift_rows(uint8_t state[4][4]) {
    uint8_t temp;
    
    // Вторая строка: сдвиг на 1 вправо
    temp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = temp;
    
    // Третья строка: сдвиг на 2 вправо
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    
    // Четвертая строка: сдвиг на 3 вправо (или 1 влево)
    temp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp;
}

uint8_t AES::gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & 1) {
            p ^= a;
        }
        bool hi_bit_set = (a & 0x80) != 0;
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return p;
}

void AES::mix_columns(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        uint8_t a[4];
        for (int j = 0; j < 4; ++j) {
            a[j] = state[j][i];
        }
        
        state[0][i] = gmul(0x02, a[0]) ^ gmul(0x03, a[1]) ^ a[2] ^ a[3];
        state[1][i] = a[0] ^ gmul(0x02, a[1]) ^ gmul(0x03, a[2]) ^ a[3];
        state[2][i] = a[0] ^ a[1] ^ gmul(0x02, a[2]) ^ gmul(0x03, a[3]);
        state[3][i] = gmul(0x03, a[0]) ^ a[1] ^ a[2] ^ gmul(0x02, a[3]);
    }
}

void AES::inv_mix_columns(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        uint8_t a[4];
        for (int j = 0; j < 4; ++j) {
            a[j] = state[j][i];
        }
        
        state[0][i] = gmul(0x0e, a[0]) ^ gmul(0x0b, a[1]) ^ gmul(0x0d, a[2]) ^ gmul(0x09, a[3]);
        state[1][i] = gmul(0x09, a[0]) ^ gmul(0x0e, a[1]) ^ gmul(0x0b, a[2]) ^ gmul(0x0d, a[3]);
        state[2][i] = gmul(0x0d, a[0]) ^ gmul(0x09, a[1]) ^ gmul(0x0e, a[2]) ^ gmul(0x0b, a[3]);
        state[3][i] = gmul(0x0b, a[0]) ^ gmul(0x0d, a[1]) ^ gmul(0x09, a[2]) ^ gmul(0x0e, a[3]);
    }
}

void AES::add_round_key(uint8_t state[4][4], int round) {
    for (int i = 0; i < 4; ++i) {
        uint32_t key_word = round_keys[round * 4 + i];
        state[0][i] ^= (key_word >> 24) & 0xFF;
        state[1][i] ^= (key_word >> 16) & 0xFF;
        state[2][i] ^= (key_word >> 8) & 0xFF;
        state[3][i] ^= key_word & 0xFF;
    }
}

void AES::encrypt_block(const uint8_t* input, uint8_t* output) {
    uint8_t state[4][4];
    bytes_to_state(input, state);
    
    // Initial round
    add_round_key(state, 0);
    
    // Main rounds (1-13)
    for (int round = 1; round < 14; ++round) {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, round);
    }
    
    // Final round (без MixColumns)
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, 14);
    
    state_to_bytes(state, output);
}

void AES::decrypt_block(const uint8_t* input, uint8_t* output) {
    uint8_t state[4][4];
    bytes_to_state(input, state);
    
    // Initial round
    add_round_key(state, 14);
    
    // Main rounds (13-1)
    for (int round = 13; round > 0; --round) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, round);
        inv_mix_columns(state);
    }
    
    // Final round (без InvMixColumns)
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, 0);
    
    state_to_bytes(state, output);
}

void AES::encrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw DESExceptions::FileReadError(input_path);
    }
    
    input.seekg(0, std::ios::end);
    uint64_t file_size = input.tellg();
    input.seekg(0, std::ios::beg);
    
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw DESExceptions::FileWriteError(output_path);
    }
    
    // Записываем оригинальный размер файла
    output.write(reinterpret_cast<const char*>(&file_size), sizeof(file_size));
    
    std::cout << "Шифрование файла...\n";
    std::cout << "Размер файла: " << file_size << " байт\n";
    
    size_t blocks_processed = 0;
    size_t total_blocks = (file_size + 15) / 16;
    
    uint8_t buffer[16];
    uint8_t encrypted[16];
    
    while (true) {
        input.read(reinterpret_cast<char*>(buffer), 16);
        size_t bytes_read = input.gcount();
        
        if (bytes_read == 0) break;
        
        // Zero padding для последнего блока
        if (bytes_read < 16) {
            std::memset(buffer + bytes_read, 0, 16 - bytes_read);
        }
        
        encrypt_block(buffer, encrypted);
        output.write(reinterpret_cast<const char*>(encrypted), 16);
        
        blocks_processed++;
        if (blocks_processed % 1000 == 0 || blocks_processed == total_blocks) {
            std::cout << "Обработано блоков: " << blocks_processed << "/" << total_blocks << "\r" << std::flush;
        }
    }
    
    std::cout << "\nШифрование завершено успешно!\n";
    
    input.close();
    output.close();
}

void AES::decrypt_file(const std::string& input_path, const std::string& output_path) {
    if (!key_generated) {
        throw DESExceptions::KeyNotGenerated();
    }
    
    std::ifstream input(input_path, std::ios::binary);
    if (!input.is_open()) {
        throw DESExceptions::FileReadError(input_path);
    }
    
    // Читаем оригинальный размер файла
    uint64_t original_size;
    input.read(reinterpret_cast<char*>(&original_size), sizeof(original_size));
    
    if (!input) {
        throw DESExceptions::DecryptionError("Неверный формат зашифрованного файла");
    }
    
    std::ofstream output(output_path, std::ios::binary);
    if (!output.is_open()) {
        throw DESExceptions::FileWriteError(output_path);
    }
    
    std::cout << "Расшифровка файла...\n";
    std::cout << "Оригинальный размер: " << original_size << " байт\n";
    
    size_t blocks_processed = 0;
    uint64_t bytes_written = 0;
    
    uint8_t buffer[16];
    uint8_t decrypted[16];
    
    while (input.read(reinterpret_cast<char*>(buffer), 16)) {
        decrypt_block(buffer, decrypted);
        
        // Определяем сколько байт записать (для последнего блока)
        size_t bytes_to_write = 16;
        if (bytes_written + 16 > original_size) {
            bytes_to_write = original_size - bytes_written;
        }
        
        output.write(reinterpret_cast<const char*>(decrypted), bytes_to_write);
        bytes_written += bytes_to_write;
        
        blocks_processed++;
        if (blocks_processed % 1000 == 0) {
            std::cout << "Обработано блоков: " << blocks_processed << "\r" << std::flush;
        }
    }
    
    std::cout << "\nРасшифровка завершена успешно!\n";
    
    input.close();
    output.close();
}