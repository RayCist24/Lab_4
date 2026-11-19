#include "money.h"

#include <limits>

Money::Money() : rubles_(0), kopeks_(0) {}

Money::Money(unsigned int rubles, unsigned int kopeks)
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

Money::Money(const Money& other)
    : rubles_(other.rubles_),
    kopeks_(other.kopeks_),
    is_valid_(other.is_valid_) {
}

Money::Money(const std::string& text)
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

Money Money::AddKopeks(unsigned int amount) const {
    Money result(*this);
    unsigned int max_rubles = std::numeric_limits<unsigned int>::max();

    unsigned int extra_rubles = amount / 100U;
    unsigned int extra_kopeks = amount % 100U;

    if (extra_rubles > max_rubles - result.rubles_) {
        result.Invalidate();
        return result;
    }
    result.rubles_ += extra_rubles;

    unsigned int total_kopeks =
        static_cast<unsigned int>(result.kopeks_) + extra_kopeks;
    if (total_kopeks >= 100U) {
        if (result.rubles_ == max_rubles) {
            result.Invalidate();
            return result;
        }
        result.rubles_ += 1U;
        total_kopeks -= 100U;
    }
    result.kopeks_ = static_cast<short int>(total_kopeks);
    return result;
}

Money& Money::operator++() {
    AddOneKopek();
    return *this;
}

Money Money::operator++(int) {
    Money old(*this);
    AddOneKopek();
    return old;
}

Money& Money::operator--() {
    SubtractOneKopek();
    return *this;
}

Money Money::operator--(int) {
    Money old(*this);
    SubtractOneKopek();
    return old;
}

Money::operator double() const {
    return static_cast<double>(kopeks_) / 100.0;
}

Money Money::operator+(unsigned int amount) const {
    return AddKopeks(amount);
}

Money Money::operator-(unsigned int amount) const {
    Money result(*this);
    if (!result.is_valid_) return result;

    unsigned int need_rubles = amount / 100U;
    unsigned int need_kopeks = amount % 100U;

    if (need_rubles > result.rubles_) {
        result.Invalidate();
        return result;
    }
    if (need_rubles == result.rubles_ &&
        need_kopeks > static_cast<unsigned int>(result.kopeks_)) {
        result.Invalidate();
        return result;
    }

    result.rubles_ -= need_rubles;
    if (static_cast<unsigned int>(result.kopeks_) >= need_kopeks) {
        result.kopeks_ = static_cast<short int>(
            static_cast<unsigned int>(result.kopeks_) - need_kopeks);
    }
    else {
        result.rubles_ -= 1U;
        result.kopeks_ = static_cast<short int>(
            static_cast<unsigned int>(result.kopeks_) + 100U - need_kopeks);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Money& money) {
    os << money.rubles_ << " руб. " << money.kopeks_ << " коп.";
    return os;
}

void Money::Invalidate() {
    rubles_ = 0;
    kopeks_ = 0;
    is_valid_ = false;
}

void Money::AddOneKopek() {
    unsigned int max_rubles = std::numeric_limits<unsigned int>::max();
    if (kopeks_ == 99) {
        if (rubles_ == max_rubles) {
            Invalidate();
            return;
        }
        rubles_ += 1U;
        kopeks_ = 0;
    }
    else {
        kopeks_ = static_cast<short int>(kopeks_ + 1);
    }
}

void Money::SubtractOneKopek() {
    if (kopeks_ > 0) {
        kopeks_ = static_cast<short int>(kopeks_ - 1);
        return;
    }
    if (rubles_ == 0) {
        Invalidate();
        return;
    }
    rubles_ -= 1U;
    kopeks_ = 99;
}

bool Money::AllDigits(const std::string& text) {
    for (char c : text) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

bool Money::ParseUnsigned(const std::string& text, unsigned int* out) {
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