#pragma once
#ifndef ENCRYPTION
#define ENCRYPTION
#define EXTENDED_ENCRYPTION
#include "includes.h"
using std::string;

class Encryption {
private:
	std::bitset<256> m_key;
	string m_iv;
	string m_encryptedData;
	string m_decryptedData;
	size_t shift_amount;
public:
	string generateKey(const size_t& length) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
		string key = string(' ', length);
		for (size_t i = 0; i < length; ++i) {
			key[i] = charset[dist(rng)];
		}
		
		m_key = bitcastString(key);
		std::cout << bitsetToString(m_key);
#ifdef EXTENDED_ENCRYPTION
		std::uniform_int_distribution<> dist2(1, 5);
		shift_amount = dist2(rng);
		m_key = m_key << shift_amount;
#endif
		return bitsetToString(m_key);
	}
	string xorCrypt(const string& passwd, const string& key) {
		string output = passwd;
		for (size_t i = 0; i < passwd.size(); ++i) {
			output[i] ^= key[i % key.size()];
		}

		return output;
	}

	const size_t getShiftAmount() const{
		return shift_amount;
	}

};

#endif
