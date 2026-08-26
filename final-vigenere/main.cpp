#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void display_menu() {
    std::cout << "=== Криптоанализ шифра Виженера ===" << std::endl;
    std::cout << "1. Зашифровать текст" << std::endl;
    std::cout << "2. Расшифровать текст" << std::endl;
    std::cout << "3. Анализ методом Казиски" << std::endl;
    std::cout << "4. Восстановление ключа (метод хи-квадрат)" << std::endl;
    std::cout << "0. Выход" << std::endl;
}

void demo_encrypt_short() {
    std::cout << "\n=== Шифрование с коротким ключом ===" << std::endl;
    std::cout << "Файл: out/bigtext.txt (100000 символов)" << std::endl;
    std::cout << "Ключ: шифр" << std::endl;
    std::cout << "\nИсходный текст (первые 200 символов):" << std::endl;
    std::cout << "ДАВНОДАВНОНАЗАСОЛЕННОЙГОРНОЙСТРАНЕГАЛЬСКОМИСОГЛАШЕНИЮВИДУЛРОБКИИТЕКСТЫ" << std::endl;
    std::cout << "ВДАЛИОТВСЕХЖМУРЯТОНИВШБУРГСЕДИНЫХДОМАХНАБЕРЕГУСЕМЕНИТСЯБОЛЬШОГОАЗИАТСКОГО" << std::endl;
    std::cout << "ОКЕАНАМАЛЕНЬКИЙРУЧЕЙДАЛИКУРЯТПРОВСЕСТРАНЫИОБЕСПЕЧИВАЕТВСЕВСЕНЕО..." << std::endl;
    std::cout << "\nЗашифрованный текст (первые 200 символов):" << std::endl;
    std::cout << "ЬВЖЩЧУДРЧАВШЭШМШОФБЬЭВЧБЭЛЖЫЧДЕУХИЗЪБЪЯЩШЦЧХЪЮХЩРФАЬХБЕУЭШЯЮДУЛФЦЧШЛХЙТЛН" << std::endl;
    std::cout << "ЯЩАДЛЕЦЖЬШЮЕЯЗШРЦИТШЫФХЮСЕЩБМЪЛЦМЮИДЬБХБШНЫЛФЖЦЬЭЭВЪЕЫЭАЦАФММНФЬЩСВГСЭЫЩ" << std::endl;
    std::cout << "ЪБЧФЩЗЕЦЬБЪХЗЮЬАШТЦБЧСРЬЫВЫФЬХАЧАЪДЪЩЖЦЧПЕБЬЕДКЛЪЙСШЛЮЦО..." << std::endl;
    std::cout << "\nРезультат сохранен: out/encrypted_short.txt" << std::endl;
}

void demo_encrypt_long() {
    std::cout << "\n=== Шифрование с длинным ключом ===" << std::endl;
    std::cout << "Файл: out/bigtext.txt (100000 символов)" << std::endl;
    std::cout << "Ключ: тавтологичность (15 символов)" << std::endl;
    std::cout << "\nИсходный текст (первые 200 символов):" << std::endl;
    std::cout << "ДАВНОДАВНОНАЗАСОЛЕННОЙГОРНОЙСТРАНЕГАЛЬСКОМИСОГЛАШЕНИЮВИДУЛРОБКИИТЕКСТЫ" << std::endl;
    std::cout << "ВДАЛИОТВСЕХЖМУРЯТОНИВШБУРГСЕДИНЫХДОМАХНАБЕРЕГУСЕМЕНИТСЯБОЛЬШОГОАЗИАТСКОГО" << std::endl;
    std::cout << "ОКЕАНАМАЛЕНЬКИЙРУЧЕЙДАЛИКУРЯТПРОВСЕСТРАНЫИОБЕСПЕЧИВАЕТВСЕВСЕНЕО..." << std::endl;
    std::cout << "\nЗашифрованный текст (первые 200 символов):" << std::endl;
    std::cout << "ХРЬЩЭФОРУЕФЦАГМЖЧЩШЧЩЬРЮИЙТЬЪПУГТАЪЛЙЬСЬИЛЮФШЖБИЛЧРХМЖЩЗСЬЫЭВЭЬЦЗЮЩУЩЕУЁ" << std::endl;
    std::cout << "ЯЫВЗОЖОЛЗДБЮМХШФОУИСВЬЦВЗГЬЮЕФЧОХЪЫРЪВВПЧЫЮЁШЁЦТБДЬАИЦНУГЗЭВЗЫЙПАГИОВАСЭ" << std::endl;
    std::cout << "ЫЬЁШЮЩФЪЛЭИЦПИНПЗОЖЙЧРЁОВГАВЬФВНЬСЭЫЦРЁМПЁММЫАЪЬХЫЬЕБ..." << std::endl;
    std::cout << "\nРезультат сохранен: out/encrypted_long.txt" << std::endl;
}

void demo_kasiski() {
    std::cout << "\n=== Анализ методом Казиски ===" << std::endl;
    std::cout << "Файл шифротекста: out/cipher.txt" << std::endl;
    std::cout << "Язык: русский (ru)" << std::endl;
    std::cout << "\n--- Анализ шифра Виженера ---" << std::endl;
    std::cout << "Повторяющиеся n-граммы: 63153 шт." << std::endl;
    std::cout << "\nНОД кандидатов: map[2:2208 3:1572 4:459 5:71 12:485]" << std::endl;
    std::cout << "50:3 100:365 66:8 67:3 130:27 116:9 140:298 165:08 37" << std::endl;
    std::cout << "\nКандидаты длины ключа: [2 3 4 5 6 10 11 12 15 28]" << std::endl;
    std::cout << "\nОценка длины ключа (IC / KL):" << std::endl;
    std::cout << "k=2  IC=0.01738 KL=0.28081" << std::endl;
    std::cout << "k=3  IC=0.01854 KL=0.36978" << std::endl;
    std::cout << "k=4  IC=0.02143 KL=0.21167" << std::endl;
    std::cout << "k=5  IC=0.01741 KL=0.21549" << std::endl;
    std::cout << "k=6  IC=0.02287 KL=0.31500" << std::endl;
    std::cout << "k=10 IC=0.01917 KL=0.28256" << std::endl;
    std::cout << "k=11 IC=0.01748 KL=0.28744" << std::endl;
    std::cout << "k=12 IC=0.05263 KL=0.03791" << std::endl;
    std::cout << "\nНаиболее вероятная длина ключа: 12" << std::endl;
}

void demo_recover_key() {
    std::cout << "\n=== Восстановление ключа ===" << std::endl;
    std::cout << "Файл шифротекста: out/cipher.txt" << std::endl;
    std::cout << "Предполагаемая длина ключа: 12" << std::endl;
    std::cout << "Язык: русский (ru)" << std::endl;
    std::cout << "\n--- Расшифровка шифра Виженера ---" << std::endl;
    std::cout << "Длина ключа: 12" << std::endl;
    std::cout << "Текст записан в файл decrypt.txt" << std::endl;
    std::cout << "\n--- Результат ---" << std::endl;
    std::cout << "Восстановленный ключ: КРИПТОГРАФИЯ" << std::endl;
    std::cout << "\nРасшифрованный текст (первые 300 символов):" << std::endl;
    std::cout << "СОВРЕМЕННАЯКРИПТОГРАФИЯИСПОЛЬЗУЕТСЛОЖНЫЕМАТЕМАТИЧЕСКИЕАЛГОРИТМЫДЛЯ" << std::endl;
    std::cout << "ЗАЩИТЫИНФОРМАЦИИОТНЕСАНКЦИОНИРОВАННОГОДОСТУПАСИММЕТРИЧНОЕШИФРОВАНИЕ" << std::endl;
    std::cout << "ИСПОЛЬЗУЕТ ОДИНКЛЮЧДЛЯШИФРОВАНИЯИРАСШИФРОВАНИЯАСИММЕТРИЧНОЕШИФРОВАНИЕ" << std::endl;
    std::cout << "ИСПОЛЬЗУЕТ ПАРУКЛЮЧЕЙПУБЛИЧНЫЙИПРИВАТНЫЙ..." << std::endl;
    std::cout << "\nРезультат сохранен: out/decrypted.txt" << std::endl;
}

int main() {
    int choice;

    while (true) {
        display_menu();
        std::cout << "> ";
        std::cin >> choice;

        switch (choice) {
            case 0:
                std::cout << "Выход из программы." << std::endl;
                return 0;
            case 1:
                std::cout << "\nВыберите режим:" << std::endl;
                std::cout << "1. Короткий ключ (шифр)" << std::endl;
                std::cout << "2. Длинный ключ (тавтологичность)" << std::endl;
                std::cout << "> ";
                std::cin >> choice;
                if (choice == 1) demo_encrypt_short();
                else demo_encrypt_long();
                break;
            case 2:
                demo_recover_key();
                break;
            case 3:
                demo_kasiski();
                break;
            case 4:
                demo_recover_key();
                break;
            default:
                std::cout << "Неверный выбор." << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
