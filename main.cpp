#define NOMINMAX
#include <windows.h>

#include <iostream>
#include <limits>
#include <string>

#include "money.h"

void ReadUnsigned(const std::string& prompt, const std::string& what,
    unsigned int* out)
{
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "Ошибка: не удалось прочитать строку.\n";
            continue;
        }
        if (line.empty()) {
            std::cout << "Ошибка: пустой ввод. Введите " << what << ".\n";
            continue;
        }
        bool negative = (line[0] == '-');
        size_t start = negative ? 1 : 0;
        bool all_digits = (start < line.size());
        for (size_t i = start; i < line.size(); ++i) {
            if (line[i] < '0' || line[i] > '9') {
                all_digits = false;
                break;
            }
        }
        if (!all_digits) {
            std::cout << "Ошибка: " << what
                << " должны быть целым числом. Повторите.\n";
            continue;
        }
        if (negative) {
            std::cout << "Ошибка: " << what
                << " не могут быть отрицательными. Повторите.\n";
            continue;
        }
        unsigned int value = 0;
        unsigned int max_value = std::numeric_limits<unsigned int>::max();
        bool overflow = false;
        for (size_t i = 0; i < line.size(); ++i) {
            unsigned int digit = static_cast<unsigned int>(line[i] - '0');
            if (value > (max_value - digit) / 10U) {
                overflow = true;
                break;
            }
            value = value * 10U + digit;
        }
        if (overflow) {
            std::cout << "Ошибка: " << what
                << " слишком велики (максимум " << max_value
                << "). Повторите.\n";
            continue;
        }
        *out = value;
        return;
    }
}

int ReadMenuChoice(int max_item)
{
    std::string line;
    std::cout << "Ваш выбор: ";
    if (!std::getline(std::cin, line)) return -1;
    if (line.empty()) {
        std::cout << "Ошибка: пустой ввод.\n";
        return -1;
    }
    for (char c : line) {
        if (c < '0' || c > '9') {
            std::cout << "Ошибка: номер пункта — целое число.\n";
            return -1;
        }
    }
    int value = 0;
    for (char c : line) {
        value = value * 10 + (c - '0');
        if (value > max_item) {
            std::cout << "Ошибка: номер пункта вне диапазона 0.."
                << max_item << ".\n";
            return -1;
        }
    }
    return value;
}

void PrintMenu()
{
    std::cout << "\n===== МЕНЮ =====\n";
    std::cout << "1.  Конструктор по умолчанию\n";
    std::cout << "2.  Параметрический конструктор (задать текущее)\n";
    std::cout << "3.  Конструктор копирования\n";
    std::cout << "4.  Конструктор из строки\n";
    std::cout << "5.  Показать текущее значение\n";
    std::cout << "6.  AddKopeks: добавить копейки\n";
    std::cout << "7.  Нормализация (0 руб. 125 коп.)\n";
    std::cout << "8.  ++m (префиксный): +1 копейка\n";
    std::cout << "9.  m++ (постфиксный): +1 копейка\n";
    std::cout << "10. --m (префиксный): -1 копейка\n";
    std::cout << "11. m-- (постфиксный): -1 копейка\n";
    std::cout << "12. (unsigned int)m: только рубли\n";
    std::cout << "13. (double)m: копейки в рублях (<1)\n";
    std::cout << "14. m + N копеек\n";
    std::cout << "15. m - N копеек\n";
    std::cout << "0.  Выход\n";
    std::cout << "================\n";
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Money current;

    while (true) {
        PrintMenu();
        int choice = ReadMenuChoice(15);

        switch (choice) {
        case 1: {
            Money zero;
            std::cout << "Конструктор по умолчанию: " << zero << "\n";
            break;
        }
        case 2: {
            unsigned int rubles = 0;
            unsigned int kopeks = 0;
            ReadUnsigned("Введите рубли: ", "рубли", &rubles);
            unsigned int max_rubles =
                std::numeric_limits<unsigned int>::max();
            if (rubles == max_rubles) {
                std::cout << "Внимание: рубли на максимуме, копейки "
                    "должны быть меньше 100.\n";
            }
            ReadUnsigned("Введите копейки: ", "копейки", &kopeks);
            Money param(rubles, kopeks);
            if (param.is_valid()) {
                std::cout << "Параметрический конструктор: "
                    << param << "\n";
                current = param;
                std::cout << "Текущее значение обновлено.\n";
            }
            else {
                std::cout << "Параметрический конструктор: переполнение\n";
            }
            break;
        }
        case 3: {
            Money copied(current);
            std::cout << "Конструктор копирования: " << copied << "\n";
            break;
        }
        case 4: {
            std::string text;
            std::cout << "Введите деньги в формате R.K "
                "(например 12.50): ";
            std::getline(std::cin, text);
            Money from_string(text);
            if (from_string.is_valid()) {
                std::cout << "Конструктор из строки: "
                    << from_string << "\n";
            }
            else {
                std::cout << "Конструктор из строки: неверный ввод\n";
            }
            break;
        }
        case 5: {
            std::cout << "Текущее значение: " << current;
            if (!current.is_valid()) std::cout << " [НЕВАЛИДНО]";
            std::cout << "\n";
            break;
        }
        case 6: {
            unsigned int to_add = 0;
            ReadUnsigned("Сколько копеек добавить? ", "копейки", &to_add);
            Money after_add = current.AddKopeks(to_add);
            if (after_add.is_valid()) {
                std::cout << "Было: " << current << "\n";
                std::cout << "Добавили " << to_add << " коп. -> "
                    << after_add << "\n";
            }
            else {
                std::cout << "AddKopeks: переполнение\n";
            }
            break;
        }
        case 7: {
            Money normalized(0, 125);
            if (normalized.is_valid()) {
                std::cout << "Нормализация (0 руб. 125 коп.): "
                    << normalized << "\n";
            }
            break;
        }
        case 8: {
            std::cout << "До: " << current << "\n";
            ++current;
            std::cout << "После ++m: " << current;
            if (!current.is_valid()) std::cout << " [НЕВАЛИДНО]";
            std::cout << "\n";
            break;
        }
        case 9: {
            std::cout << "До: " << current << "\n";
            Money old = current++;
            std::cout << "Возвращено (старое): " << old << "\n";
            std::cout << "После m++: " << current;
            if (!current.is_valid()) std::cout << " [НЕВАЛИДНО]";
            std::cout << "\n";
            break;
        }
        case 10: {
            std::cout << "До: " << current << "\n";
            --current;
            std::cout << "После --m: " << current;
            if (!current.is_valid()) {
                std::cout << " [НЕВАЛИДНО: ушли бы в минус]";
            }
            std::cout << "\n";
            break;
        }
        case 11: {
            std::cout << "До: " << current << "\n";
            Money old = current--;
            std::cout << "Возвращено (старое): " << old << "\n";
            std::cout << "После m--: " << current;
            if (!current.is_valid()) {
                std::cout << " [НЕВАЛИДНО: ушли бы в минус]";
            }
            std::cout << "\n";
            break;
        }
        case 12: {
            unsigned int rubles_only =
                static_cast<unsigned int>(current);
            std::cout << "Текущее: " << current << "\n";
            std::cout << "(unsigned int)m = " << rubles_only
                << " (копейки отброшены)\n";
            break;
        }
        case 13: {
            double fraction = static_cast<double>(current);
            std::cout << "Текущее: " << current << "\n";
            std::cout << "(double)m = " << fraction
                << " (дробная часть рублей)\n";
            break;
        }
        case 14: {
            unsigned int to_add = 0;
            ReadUnsigned("Сколько копеек прибавить? ", "копейки", &to_add);
            Money result = current + to_add;
            std::cout << "Текущее: " << current << "\n";
            std::cout << "m + " << to_add << " = " << result;
            if (!result.is_valid()) {
                std::cout << " [НЕВАЛИДНО: переполнение]";
            }
            std::cout << "\n";
            break;
        }
        case 15: {
            unsigned int to_sub = 0;
            ReadUnsigned("Сколько копеек вычесть? ", "копейки", &to_sub);
            Money result = current - to_sub;
            std::cout << "Текущее: " << current << "\n";
            if (result.is_valid()) {
                std::cout << "m - " << to_sub << " = " << result << "\n";
            }
            else {
                std::cout << "m - " << to_sub
                    << ": НЕВАЛИДНО (ушли бы в минус)\n";
            }
            break;
        }
        case 0: {
            std::cout << "Выход.\n";
            return 0;
        }
        default: {
            break;
        }
        }
    }
}