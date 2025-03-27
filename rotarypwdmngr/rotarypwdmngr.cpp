#include "includes.h"
int main()
{
	int index;
	initChecks();
	std::vector<std::string> keys = loadKeysToMemory();
	std::vector<std::string> passwords = loadPasswordsToMemory();
	std::vector<std::string> passwordelements = loadPasswordElementsToMemory();
	getPasswords();
	Sleep(10000);
	return 0;
}


