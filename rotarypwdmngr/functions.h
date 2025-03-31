#pragma once
inline encryption enc;

using std::string;
inline string key;
inline int keysize = 32;


inline void center_next_im_gui() {
	const float available_width = ImGui::GetContentRegionAvail().x;
	const float pos_x = (available_width - 800) * 0.5f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pos_x);
}

inline void get_valid_index_input(int& input, const int min, const int max) {
	while (true) {
		std::cout << "Enter the website number between " << min << " and " << max << ": ";
		std::cin >> input;

		// Check if input is a valid integer
		if (std::cin.fail()) {
			std::cin.clear();  // Clear error flag
			std::cin.ignore(1000, '\n');  // Discard invalid input
			std::cout << "Invalid input! Please enter a number.\n";
		}
		else if (input >= min && input <= max) {
			break;  // Exit loop if input is within range
		}
		else {
			std::cout << "Invalid range! Please enter a number between " << min << " and " << max << ".\n";
		}
	}
}

inline void get_non_empty_input(std::string& input, const std::string& prompt) {
	while (true) {
		std::cout << prompt;
		std::getline(std::cin, input);

		if (!input.empty()) {
			break;
		}

		std::cout << "Input cannot be empty! Please try again.\n";
	}
}

inline std::vector<std::string> load_keys_to_memory() {
	std::vector<std::string> keys = file_manager::read_from_file_line_by_line("keys.txt");

	/*std::cout << "Keys from file:" << "\n";
	for (size_t i = 0; i < keys.size(); ++i) {
		std::cout << "Keys " << i + 1 << ": " << keys[i] << "\n";
	}*/

	return keys;
}

inline std::vector<std::string> load_passwords_to_memory() {
	std::vector<std::string> passwords = file_manager::read_from_file_line_by_line("passwords.txt");

	/*std::cout << "Passwords from file:" << "\n";
	for (size_t i = 0; i < passwords.size(); ++i) {
		std::cout << "Pass " << i + 1 << ": " << passwords[i] << "\n";
	} */

	return passwords;
}

inline std::vector<std::string> load_password_elements_to_memory() {
	std::vector<std::string> passwordelements = file_manager::read_from_file_line_by_line("passwordelements.txt");

	/*std::cout << "Passwords from file:" << "\n";
	for (size_t i = 0; i < passwords.size(); ++i) {
		std::cout << "Pass " << i + 1 << ": " << passwords[i] << "\n";
	} */

	return passwordelements;
}

inline void add_password(const string& passwordelements, const string& password) {
	const std::vector<std::string> keys = load_keys_to_memory();
	const string encryptedpass = encryption::xorCrypt(password, keys[0]);
	file_manager::write_to_next_line("passwords.txt", encryptedpass);
	file_manager::write_to_next_line("passwordelements.txt", passwordelements);
}

inline void delete_password(const string& filename, const int line_to_delete) {
	file_manager::delete_specific_line(filename, line_to_delete);
}

inline string decrypt_password(const int line) {
	const std::vector<std::string> keys = load_keys_to_memory();
	const std::vector<std::string> passwords = load_passwords_to_memory();
	string decryptedpassword = encryption::xorCrypt(passwords[line], keys[0]);
	return decryptedpassword;

}

inline void get_websites() {
	std::vector<std::string> passwords = load_passwords_to_memory();
	const std::vector<std::string> passwordelements = load_password_elements_to_memory();
	std::cout << "Websites You Saved:" << "\n";
	for (size_t i = 0; i < passwordelements.size(); ++i) {
		std::cout << "[" << i + 1 << "]: " << passwordelements[i] << "\n";
	}
}

inline void init_checks() {

	if (file_manager::does_file_exist("keys.txt")) {
		key = file_manager::read_from_file("keys.txt");
		if (key.empty()) {
			key = enc.generate_key(32);
			file_manager::write_to_file("keys.txt", key);

		}
	}
	else {
		key = enc.generate_key(32);
		file_manager::create_file("keys.txt");
		file_manager::write_to_file("keys.txt", key);
	}
	if (!file_manager::does_file_exist("passwords.txt")) {
		file_manager::create_file("passwords.txt");
	}
	if (!file_manager::does_file_exist("passwordelements.txt")) {
		file_manager::create_file("passwordelements.txt");
	}

}