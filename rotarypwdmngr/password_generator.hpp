#pragma once
#ifndef PASSWORD_GENERATOR
#define PASSWORD_GENERATOR

#include <random>
#include <string>
#include <vector>

// Function to strengthen the password
void strengthen_password(std::string& m_password);

class password_generator {
public:
    password_generator() = default;
    ~password_generator() = default;

    // Rule of Five: Delete copy/move constructors if not needed
    password_generator(const password_generator&) = delete;
    password_generator& operator=(const password_generator&) = delete;
    password_generator(password_generator&&) = delete;
    password_generator& operator=(password_generator&&) = delete;

    // Generate a random password of specified length
    static std::string generate_password(const uint32_t& length = 10) {
        auto password = std::string(" ", length);
        for (uint32_t i = 0; i < length; i++) {
            password[i] = ascii_pool[random_int_range()];
        }
        strengthen_password(password);
        return password;
    }

private:
    static inline const std::string ascii_pool = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";

    // Thread-safe random integer generator
    static uint32_t random_int_range(const uint32_t min = 0, const uint32_t max = 61) {
        thread_local std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution distribution(min, max);
        return distribution(generator);
    }
};

// Function to strengthen the password
inline void strengthen_password(std::string& m_password) {
    if (entropy_bits(m_password) >= max_entropy(m_password.size())) // if the password is already strong discards the function
        return;

    vector2<uint32_t> instruction(1, 0); // the pools to replace
    const std::vector<uint32_t> password_stats = char_stats(m_password);

    for (uint32_t i = 0; i < password_stats.size(); i++) {
        if (password_stats[i] > instruction.x)
            instruction.x = i; // finds the most common char type
        else if (password_stats[i] == 0)
            instruction.y = i; // finds the missing char type
    }

    for (char& i : m_password) //https://clang.llvm.org/extra/clang-tidy/checks/modernize/loop-convert.html
    {
        if (get_char_type(i) == instruction.x) {  // NOLINT(clang-diagnostic-sign-compare)
            if (instruction.y == 0)
                i = upper_pool[random_int_range(0, upper_length)]; // if the missing char type is an upper case char
            else if (instruction.y == 1)
                i = lower_pool[random_int_range(0, lower_length)]; // if the missing char type is a lower case char
            else if (instruction.y == 2)
                i = digit_pool[random_int_range(0, digit_length)]; // if the missing char type is a digit char
            else if (instruction.y == 3)
                i = symbol_pool[random_int_range(0, symbol_length)]; // if the missing char type is a symbol char
            break;
        }
    }
    remove_adjacent_duplicates(m_password);
    if (entropy_bits(m_password) < max_entropy(m_password.length()))
        strengthen_password(m_password);
}

#endif
