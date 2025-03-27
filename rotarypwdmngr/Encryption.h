#pragma once
#ifndef ENCRYPTION
#define ENCRYPTION
#include "includes.h"
using std::string;

class Encryption {
private:
	string m_key;
	string m_iv;
	string m_encryptedData;
	string m_decryptedData;
public:
	string generateKey(const size_t& length) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
		std::uniform_int_distribution<> dist2(0, 10);
		string key = string(' ', length);
		for (size_t i = 0; i < length; ++i) {
			key = charset[dist(rng)];
		}
		
		std::bitset<256> a = bitcastString(key);

		std::cout << a << std::endl;
		for (size_t i = 0; i < dist2(rng); i++) {
			a = a << 1;
		}
		std::cout << a << std::endl;
		return key;
	}
	string xorCrypt(const string& passwd, const string& key) {
		string output = passwd;
		for (size_t i = 0; i < passwd.size(); ++i) {
			output[i] ^= key[i % key.size()];
		}

		return output;
	}


};

#endif
