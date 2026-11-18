#include <iostream>
#include <limits>
#include <string>

class Money {
public:

    Money() : rubles_(0), kopeks_(0) {}

    Money(unsigned int rubles, unsigned int kopeks)
        : rubles_(rubles), kopeks_(0) {
        unsigned int extra_rubles = kopeks / 100U;
        if (extra_rubles >
            std::numeric_limits<unsigned int>::max() - rubles_) {
            rubles_ = 0;
            kopeks_ = 0;
            is_valid_ = false;
            return;
        }
        rubles_ += extra_rubles;
        kopeks_ = static_cast<short int>(kopeks % 100U);
    }

    Money(const Money& other)
        : rubles_(other.rubles_),
        kopeks_(other.kopeks_),
        is_valid_(other.is_valid_) {
    }

    explicit Money(const std::string& text)
        : rubles_(0), kopeks_(0), is_valid_(false) {
        size_t dot_pos = text.find('.');
        std::string rubles_str =
            (dot_pos == std::string::npos) ? text : text.substr(0, dot_pos);
        std::string kopeks_str =
            (dot_pos == std::string::npos) ? "0" : text.substr(dot_pos + 1);

        if (rubles_str.empty() || kopeks_str.empty()) return;
        if (kopeks_str.size() > 2) return;
        if (!AllDigits(rubles_str)) return;
        if (!AllDigits(kopeks_str)) return;

        unsigned int rubles = 0;
        if (!ParseUnsigned(rubles_str, &rubles)) return;

        unsigned int kopeks = 0;
        if (!ParseUnsigned(kopeks_str, &kopeks)) return;
        if (kopeks > 99U) return;

        rubles_ = rubles;
        kopeks_ = static_cast<short int>(kopeks);
        is_valid_ = true;
    }

    unsigned int rubles() const { return rubles_; }
    short int kopeks() const { return kopeks_; }
    bool is_valid() const { return is_valid_; }

    Money AddKopeks(unsigned int amount) const {
        Money result(*this);
        unsigned int max_rubles = std::numeric_limits<unsigned int>::max();

        unsigned int extra_rubles = amount / 100U;
        unsigned int extra_kopeks = amount % 100U;

        if (extra_rubles > max_rubles - result.rubles_) {
            result.rubles_ = 0;
            result.kopeks_ = 0;
            result.is_valid_ = false;
            return result;
        }
        result.rubles_ += extra_rubles;

        unsigned int total_kopeks =
            static_cast<unsigned int>(result.kopeks_) + extra_kopeks;
        if (total_kopeks >= 100U) {
            if (result.rubles_ == max_rubles) {
                result.rubles_ = 0;
                result.kopeks_ = 0;
                result.is_valid_ = false;
                return result;
            }
            result.rubles_ += 1U;
            total_kopeks -= 100U;
        }
        result.kopeks_ = static_cast<short int>(total_kopeks);
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const Money& money) {
        os << money.rubles_ << " руб. " << money.kopeks_ << " коп.";
        return os;
    }

private:
    static bool AllDigits(const std::string& text) {
        for (char c : text) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    }

    static bool ParseUnsigned(const std::string& text, unsigned int* out) {
        unsigned int value = 0;
        unsigned int max_value = std::numeric_limits<unsigned int>::max();
        for (char c : text) {
            unsigned int digit = static_cast<unsigned int>(c - '0');
            if (value > (max_value - digit) / 10U) return false;
            value = value * 10U + digit;
        }
        *out = value;
        return true;
    }

    unsigned int rubles_;
    short int kopeks_;
    bool is_valid_ = true;
};

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
                << " слишком велики (максимум "
                << max_value << "). Повторите.\n";
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
    std::cout << "1. Конструктор по умолчанию\n";
    std::cout << "2. Параметрический конструктор (задать текущее)\n";
    std::cout << "3. Конструктор копирования\n";
    std::cout << "4. Конструктор из строки\n";
    std::cout << "5. Показать текущее значение\n";
    std::cout << "6. Добавить копейки (AddKopeks)\n";
    std::cout << "7. Нормализация (0 руб. 125 коп.)\n";
    std::cout << "0. Выход\n";
    std::cout << "================\n";
}

int main()
{
    setlocale(LC_ALL, "Russian");
    Money current;

    while (true) {
        PrintMenu();
        int choice = ReadMenuChoice(7);

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
                    "должны быть меньше 100, иначе будет "
                    "переполнение.\n";
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
                std::cout << "Параметрический конструктор: переполнение "
                    "(слишком много рублей и копеек в сумме)\n";
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
            std::cout << "Текущее значение: " << current << "\n";
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
                std::cout << "(исходное значение не изменилось)\n";
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