std::vector<int> Kasiski::find_key_lengths(const std::string& cipher,
                                           int min_gram) {
    std::map<std::string, std::vector<int>> ngram_positions;

    // Поиск повторяющихся n-грамм
    for (size_t i = 0; i <= cipher.length() - min_gram; i++) {
        std::string ngram = cipher.substr(i, min_gram);
        ngram_positions[ngram].push_back(i);
    }

    // Вычисление расстояний между повторениями
    std::vector<int> distances;
    for (auto& pair : ngram_positions) {
        if (pair.second.size() > 1) {
            for (size_t i = 1; i < pair.second.size(); i++) {
                distances.push_back(pair.second[i] - pair.second[i-1]);
            }
        }
    }

    // Вычисление НОД всех расстояний
    std::map<int, int> gcd_counts;
    for (int d : distances) {
        for (int divisor = 2; divisor <= d; divisor++) {
            if (d % divisor == 0) {
                gcd_counts[divisor]++;
            }
        }
    }

    // Возврат наиболее вероятных длин ключа
    std::vector<int> candidates;
    for (auto& pair : gcd_counts) {
        if (pair.second >= 3) {
            candidates.push_back(pair.first);
        }
    }
    return candidates;
}
