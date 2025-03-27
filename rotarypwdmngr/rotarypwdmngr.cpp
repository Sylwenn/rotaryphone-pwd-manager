#include "includes.h"
Encryption enc;
FileManager file;
PasswordGenerator passgen;
using std::string;
string key;
void DebugFunc()
{

	if (file.doesFileExists("keys.txt"))
	{
		key = file.readFromFile("keys.txt");
	}else 
	{
		key = enc.generateKey(32);
		file.createFile("keys.txt");
		file.writeToFile("keys.txt", key);
	}
	string password = passgen.generate_password(16);
	string encrypted = enc.xorCrypt(password, key);
	string decrypted = enc.xorCrypt(encrypted, key);
	std::cout << "Original " << password << std::endl;
	std::cout << "Encrypted " << encrypted << std::endl;
	std::cout << "Decrypted " << decrypted << std::endl;
	std::cout << "Your Key is " << key << std::endl;
	file.createFile("keys.txt");
	file.createFile("passwords.txt");
	file.createFile("decrypted.txt");
	file.writeToFile("keys.txt", key);
	file.writeToFile("passwords.txt", encrypted);
	string getKey = file.readFromFile("keys.txt");
	string getPasswords = file.readFromFile("passwords.txt");
	string getDecrypted = enc.xorCrypt(getPasswords, getKey);
	file.writeToFile("decrypted.txt", getDecrypted);
	string decryptedPasswords = file.readFromFile("decrypted.txt");
	std::cout << "Key from file " << getKey << std::endl;
	std::cout << "Password from file " << getPasswords << std::endl;
	std::cout << "Decrypted Password from file " << decryptedPasswords << std::endl;

}

int main()
{
	std::cout << "Hello World!\n";
	DebugFunc();
	return 0;
}


