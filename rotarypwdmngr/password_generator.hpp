#pragma once

#ifndef PASSWORD_GENERATOR
#define PASSWORD_GENERATOR
namespace generator {

	class Password_generator {
	public:
		void generate_password(const uint32_t& length) {
			for (uint32_t i = 0; i < length; i++)
				m_password[i] = ascii_pool[random_int_range()];
		}

	private:
		std::string m_password;
		uint32_t m_entropy_bits;
		void strengthen_password() {
			if (entropy_bits(m_password) >= max_entropy(m_password.size())) // if the password is already strong discards the function
				return;

			Vector2<uint32_t> instruction(1, 0); // the pools to replace
			std::vector<uint32_t> password_stats = char_stats(m_password);

			for (uint32_t i = 0; i < password_stats.size(); i++) {
				if (password_stats[i] > instruction.x)
					instruction.x = i; // finds the most common char type
				else if (password_stats[i] == 0)
					instruction.y = i; // finds the missing char type
			}

			for (uint32_t i = 0; i < m_password.size(); i++) {
				if (get_char_type(m_password[i]) == instruction.x) {
					if (instruction.y == 0)
						m_password[i] = upper_pool[random_int_range(0, upper_length)]; // if the missing char type is a upper case char
					else if (instruction.y == 1)
						m_password[i] = lower_pool[random_int_range(0, lower_length)]; // if the missing char type is a lower case char
					else if (instruction.y == 2)
						m_password[i] = digit_pool[random_int_range(0, digit_length)]; // if the missing char type is a digit char
					else if (instruction.y == 3)
						m_password[i] = symbol_pool[random_int_range(0, symbol_length)]; // if the missing char type is a symbol char
					break;
				}
			}
			remove_adjacent_duplicates(m_password);
			if (entropy_bits(m_password) < max_entropy(m_password.length()))
				strengthen_password();
		}
	};
}
#endif