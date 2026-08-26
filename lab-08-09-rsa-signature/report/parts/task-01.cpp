void RSA::generate_keys() {
    std::cout << "Генерация пары ключей RSA...\n";

    // Генерация двух различных простых чисел
    std::random_device rd;
    std::mt19937_64 gen(rd());

    p = generate_prime(10000, 50000);
    do {
        q = generate_prime(10000, 50000);
    } while (q == p);

    // Вычисление модуля n = p * q
    n = p * q;

    // Вычисление функции Эйлера φ(n) = (p-1)(q-1)
    uint64_t phi = (p - 1) * (q - 1);

    // Выбор публичной экспоненты e (обычно 65537)
    e = 65537;
    if (e >= phi || gcd(e, phi) != 1) {
        // Если 65537 не подходит, ищем другое значение
        for (e = 3; e < phi; e += 2) {
            if (gcd(e, phi) == 1) {
                break;
            }
        }
    }

    // Вычисление приватной экспоненты d (обратное к e по модулю φ(n))
    d = mod_inverse(e, phi);

    keys_generated = true;

    std::cout << "Ключи успешно сгенерированы.\n";
    std::cout << "Размер модуля: " << n << "\n\n";
}
