// Модульное возведение в степень (быстрое возведение в степень)
uint64_t RSA::mod_pow(uint64_t base, uint64_t exp, uint64_t mod) const {
    uint64_t result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (__uint128_t)result * base % mod;
        }
        exp = exp >> 1;
        base = (__uint128_t)base * base % mod;
    }

    return result;
}

// Простая хэш-функция (DJB2)
uint32_t RSA::compute_hash(const std::vector<uint8_t>& data) const {
    uint32_t hash = 5381;

    for (uint8_t byte : data) {
        hash = ((hash << 5) + hash) + byte;  // hash * 33 + byte
    }

    return hash;
}

// НОД (алгоритм Евклида)
uint64_t RSA::gcd(uint64_t a, uint64_t b) const {
    while (b != 0) {
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Модульное обратное (расширенный алгоритм Евклида)
uint64_t RSA::mod_inverse(uint64_t a, uint64_t m) const {
    int64_t m0 = m;
    int64_t x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        int64_t q = a / m;
        int64_t t = m;

        m = a % m;
        a = t;
        t = x0;

        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;

    return x1;
}
