#pragma once
#include "includes.h"


Encryption enc;
FileManager file;
PasswordGenerator passgen;
using std::string;
string key;
int KEYSIZE = 32;



void getValidIndexInput(int& input, int min, int max) {
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

void getNonEmptyInput(std::string& input, const std::string& prompt) {
	while (true) {
		std::cout << prompt;
		std::getline(std::cin, input);

		if (!input.empty()) {
			break; 
		}

		std::cout << "Input cannot be empty! Please try again.\n";
	}
}
std::vector<std::string> loadKeysToMemory()
{
	std::vector<std::string> keys = file.readFromFileLineByLine("keys.txt");

	/*std::cout << "Keys from file:" << std::endl;
	for (size_t i = 0; i < keys.size(); ++i) {
		std::cout << "Keys " << i + 1 << ": " << keys[i] << std::endl;
	}*/

	return keys;
}

std::vector<std::string> loadPasswordsToMemory()
{
	std::vector<std::string> passwords = file.readFromFileLineByLine("passwords.txt");

	/*std::cout << "Passwords from file:" << std::endl;
	for (size_t i = 0; i < passwords.size(); ++i) {
		std::cout << "Pass " << i + 1 << ": " << passwords[i] << std::endl;
	} */

	return passwords;
}
std::vector<std::string> loadPasswordElementsToMemory()
{
	std::vector<std::string> passwordelements = file.readFromFileLineByLine("passwordelements.txt");

	/*std::cout << "Passwords from file:" << std::endl;
	for (size_t i = 0; i < passwords.size(); ++i) {
		std::cout << "Pass " << i + 1 << ": " << passwords[i] << std::endl;
	} */

	return passwordelements;
}

void addPassword(string passwordelements, string password)
{
	std::vector<std::string> keys = loadKeysToMemory();
	string encryptedpass = enc.xorCrypt(password, keys[0]);
	file.writeToNextLine("passwords.txt", encryptedpass);
	file.writeToNextLine("passwordelements.txt", passwordelements);
}

string decryptPassword(int line)
{
	std::vector<std::string> keys = loadKeysToMemory();
	std::vector<std::string> passwords = loadPasswordsToMemory();
	string decryptedpassword = enc.xorCrypt(passwords[line], keys[0]);
	return decryptedpassword;

}

void getWebsites()
{
	std::vector<std::string> passwords = loadPasswordsToMemory();
	std::vector<std::string> passwordelements = loadPasswordElementsToMemory();
	std::cout << "Websites You Saved:" << std::endl;
	for (size_t i = 0; i < passwordelements.size(); ++i) {
		std::cout << "[" << i + 1 << "]: " << passwordelements[i] << std::endl;
	}
}
void decryptSpecificPassword()
{
	std::vector<std::string> passwords = loadPasswordsToMemory();
	std::vector<std::string> passwordelements = loadPasswordElementsToMemory();
	int index;
	getValidIndexInput(index, 1, passwordelements.size());
	string decryptedpassword = decryptPassword(index - 1);
	std::cout << "Password for " << passwordelements[index - 1] << " is: " << decryptedpassword << std::endl;
}


void initChecks()
{

	if (file.doesFileExist("keys.txt")) {
		key = file.readFromFile("keys.txt");
		if (key.empty())
		{
			key = enc.generateKey(32);
			file.writeToFile("keys.txt", key);

		}
	}
	else {
		key = enc.generateKey(32);
		file.createFile("keys.txt");
		file.writeToFile("keys.txt", key);
	}
	if (!file.doesFileExist("passwords.txt")) {
		file.createFile("passwords.txt");
	}
	if (!file.doesFileExist("passwordelements.txt")) {
		file.createFile("passwordelements.txt");
		addPassword("Test1", "Pass1");
		addPassword("Test2", "Pass2");
		addPassword("Test3", "Pass3");
	}

}