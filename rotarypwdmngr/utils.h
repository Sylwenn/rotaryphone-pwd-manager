#pragma once
#ifndef UTILS
#define UTILS

const char ascii_pool[] = "!\"#$%&\'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz";
constexpr uint32_t ascii_length = sizeof(ascii_pool) / sizeof(char);

const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const char upper_pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char lower_pool[] = "abcdefghijklmnopqrstuvwxyz";
const char digit_pool[] = "0123456789";
const char symbol_pool[] = "!\"#$%&\'()*+,-./:;<=>?";

constexpr uint32_t upper_length = sizeof(upper_pool) / sizeof(char);
constexpr uint32_t lower_length = sizeof(lower_pool) / sizeof(char);
constexpr uint32_t digit_length = sizeof(digit_pool) / sizeof(char);
constexpr uint32_t symbol_length = sizeof(symbol_pool) / sizeof(char);

// Tools and Util

template <typename Ty_>
struct Vector2 {
public:
	Ty_ x, y;
	Vector2(Ty_ a, Ty_ b) : x(a), y(b) {}
};

enum char_type { // char type
	upper_,
	lower_,
	digit_,
	symbol_
};

bool is_value_within_range(const int lower_bound, const int value, const int upper_bound) {
	if (lower_bound <= value && value <= upper_bound)
		return true;
	return false;
}

// compares the ascii value of char to ranges of different char types
int get_char_type(const char character) {
	bool is_uppercase = is_value_within_range(65, int(character), 90);
	bool is_lowercase = is_value_within_range(97, int(character), 122);
	bool is_digit = is_value_within_range(48, int(character), 57);
	bool is_symbol_char =
		is_value_within_range(33, int(character), 47) ||
		is_value_within_range(58, int(character), 64) ||
		is_value_within_range(91, int(character), 96) ||
		is_value_within_range(123, int(character), 126);
	if (is_uppercase)
		return upper_;

	else if (is_lowercase)
		return lower_;

	else if (is_digit)
		return digit_;

	else if (is_symbol_char)
		return symbol_;
	return upper_;
};

std::vector<uint32_t> char_stats(const std::string& str) {
	uint32_t upper_count = NULL, lower_count = NULL, digit_count = NULL, symbol_count = NULL;
	for (uint32_t i = 0; i < str.size(); i++)
	{
		if (get_char_type(str[i]) == 0)      // if uppercase
			upper_count++;

		else if (get_char_type(str[i]) == 1) // if lowercase
			lower_count++;

		else if (get_char_type(str[i]) == 2) // if digit
			digit_count++;

		else if (get_char_type(str[i]) == 3) // if symbol
			symbol_count++;
	}
	return { upper_count, lower_count, digit_count, symbol_count };
}

// returns the maximum possible entropy of a string wih that length
double max_entropy(const size_t& password_length) { return password_length * log2(94); }

double entropy_bits(const std::string& str) {
	uint32_t pool_size = 0;

	// ignore states to prevent overaddition to pool_size
	bool ignore_symbol_chars = 0, ignore_digits = 0, ignore_uppercase = 0, ignore_lowercase = 0;

	for (uint32_t i = 0; i < str.size(); i++)
	{
		char_type i_char = (char_type)get_char_type(str[i]);
		if (i_char == 0 && !ignore_uppercase)
		{
			pool_size += 26;
			ignore_uppercase = 1;
		}
		else if (i_char == 1 && !ignore_lowercase)
		{
			pool_size += 26;
			ignore_lowercase = 1;
		}
		else if (i_char == 2 && !ignore_digits)
		{
			pool_size += 10;
			ignore_digits = 1;
		}
		else if (i_char == 3 && !ignore_symbol_chars)
		{
			pool_size += 32;
			ignore_symbol_chars = 1;
		}
	}

	return str.size() * log2(pool_size);
}

template<typename T>
int random_int(const T& seed = 0) {
	if (seed == 0)
		T seed = (uint32_t)std::chrono::steady_clock::now().time_since_epoch().count();

	std::default_random_engine engine(seed);
	std::uniform_int_distribution<int> distribution(0, 1);
	return distribution(engine);
}

int random_int_range(const int& lower_bound = 0, const int& upper_bound = ascii_length) {
	if (lower_bound >= upper_bound)
		throw lower_bound, upper_bound;
	uint32_t seed = (uint32_t)std::chrono::steady_clock::now().time_since_epoch().count();
	std::default_random_engine engine(seed);
	std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
	return distribution(engine);
};

bool has_adjacent_duplicates(std::string& str) {
	for (size_t i = 1; i < str.size() - 1; i++)
		if (str[i] == str[i + 1] || str[i] == str[i - 1])
			return true;
	return false;
}

void remove_adjacent_duplicates(std::string& str) {
	if (has_adjacent_duplicates(str)) {
		for (size_t i = 1; i < str.size() - 1; i++) {
			while (str[i] == str[i + 1] || str[i] == str[i - 1])
				str[i] = ascii_pool[random_int_range()];
		}
	}
}

bool isBinary(const std::string& data)
{
	for (char ch : data) {
		if (ch < 32 || ch > 126) {
			return true;
		}
	}
	return false;
}



#endif