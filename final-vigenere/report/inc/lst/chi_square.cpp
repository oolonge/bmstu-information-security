const double RUSSIAN_FREQ[32] = {
    0.0801, 0.0159, 0.0454, 0.0047, 0.0298, 0.0845, 0.0004,
    0.0094, 0.0165, 0.0735, 0.0121, 0.0349, 0.0440, 0.0321,
    0.0670, 0.1097, 0.0281, 0.0473, 0.0547, 0.0626, 0.0262,
    0.0026, 0.0097, 0.0048, 0.0144, 0.0073, 0.0036, 0.0190,
    0.0174, 0.0032, 0.0064, 0.0201
};

double chi_square(const std::string& text, int shift) {
    std::map<int, int> freq;
    int total = 0;

    for (char ch : text) {
        if (isalpha(ch)) {
            int idx = (get_char_index(ch) - shift + 32) % 32;
            freq[idx]++;
            total++;
        }
    }

    double chi = 0.0;
    for (int i = 0; i < 32; i++) {
        double observed = freq[i];
        double expected = RUSSIAN_FREQ[i] * total;
        if (expected > 0) {
            chi += pow(observed - expected, 2) / expected;
        }
    }
    return chi;
}

std::string recover_key(const std::string& cipher, int key_len) {
    std::string key;

    for (int col = 0; col < key_len; col++) {
        std::string column;
        for (size_t i = col; i < cipher.length(); i += key_len) {
            column += cipher[i];
        }

        int best_shift = 0;
        double best_chi = 1e9;

        for (int shift = 0; shift < 32; shift++) {
            double chi = chi_square(column, shift);
            if (chi < best_chi) {
                best_chi = chi;
                best_shift = shift;
            }
        }

        key += get_char_by_index(best_shift, true);
    }
    return key;
}
