double calculate_ic(const std::string& text) {
    std::map<char, int> freq;
    int total = 0;

    for (char ch : text) {
        if (isalpha(ch)) {
            freq[toupper(ch)]++;
            total++;
        }
    }

    if (total <= 1) return 0.0;

    double ic = 0.0;
    for (auto& pair : freq) {
        int f = pair.second;
        ic += f * (f - 1);
    }
    ic /= (total * (total - 1));

    return ic;
}

int find_best_key_length(const std::string& cipher, int max_len) {
    double target_ic = 0.0553; // Для русского языка
    int best_len = 1;
    double best_diff = 1.0;

    for (int len = 1; len <= max_len; len++) {
        double avg_ic = 0.0;

        for (int col = 0; col < len; col++) {
            std::string column;
            for (size_t i = col; i < cipher.length(); i += len) {
                column += cipher[i];
            }
            avg_ic += calculate_ic(column);
        }
        avg_ic /= len;

        if (abs(avg_ic - target_ic) < best_diff) {
            best_diff = abs(avg_ic - target_ic);
            best_len = len;
        }
    }
    return best_len;
}
