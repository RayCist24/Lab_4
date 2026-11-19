#ifndef LAB_MONEY_MONEY_H_
#define LAB_MONEY_MONEY_H_

#include <iostream>
#include <string>

class Money {
public:
	Money();
	Money(unsigned int rubles, unsigned int kopeks);
	Money(const Money& other);
	explicit Money(const std::string& text);

	unsigned int rubles() const { return rubles_; }
	short int kopeks() const { return kopeks_; }
	bool is_valid() const { return is_valid_; }

	Money AddKopeks(unsigned int amount) const;

	Money& operator++();
	Money operator++(int);
	Money& operator--();
	Money operator--(int);

	explicit operator unsigned int() const { return rubles_; }
	operator double() const;

	Money operator+(unsigned int amount) const;
	Money operator-(unsigned int amount) const;

	friend std::ostream& operator<<(std::ostream& os, const Money& money);

private:
	void Invalidate();
	void AddOneKopek();
	void SubtractOneKopek();

	static bool AllDigits(const std::string& text);
	static bool ParseUnsigned(const std::string& text, unsigned int* out);

	unsigned int rubles_;
	short int kopeks_;
	bool is_valid_ = true;
};

#endif