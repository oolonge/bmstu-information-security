std::string VigenereCipher::encrypt(const std::string& text,
                                    const std::string& key) {
    std::string result;
    int key_index = 0;

    for (char ch : text) {
        if (isalpha(ch)) {
            int shift = get_shift(key[key_index % key.length()]);
            char base = isupper(ch) ? 'A' : 'a';
            int alphabet_size = 32; // Русский алфавит

            int char_index = get_char_index(ch);
            int new_index = (char_index + shift) % alphabet_size;
            result += get_char_by_index(new_index, isupper(ch));

            key_index++;
        } else {
            result += ch;
        }
    }
    return result;
}
