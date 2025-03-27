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
		const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, chars.size() - 1);

		std::string key;
		for (int i = 0; i < length; ++i) {
			key += chars[distr(gen)];
		}
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
