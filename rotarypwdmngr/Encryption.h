#pragma once
#ifndef ENCRYPTION
#define ENCRYPTION
#define EXTENDED_ENCRYPTION
using std::string;

class encryption {
	std::bitset<256> m_key_;
	string m_iv_;
	size_t shift_amount_ = 0;
public:
	string generate_key(const size_t& length) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
		auto key = string(length, ' ');
		for (size_t i = 0; i < length; ++i) {
			key[i] = charset[dist(rng)];
		}

		m_key_ = bitcastString(key);
		std::cout << bitset_to_string(m_key_);
#ifdef EXTENDED_ENCRYPTION
		std::uniform_int_distribution dist2(1, 5);
		shift_amount_ = dist2(rng);
		m_key_ = m_key_ << shift_amount_;
#endif
		return bitset_to_string(m_key_);
	}

	static string xorCrypt(const string& passwd, const string& key) {
		string output = passwd;
		for (size_t i = 0; i < passwd.size(); ++i) {
			output[i] ^= key[i % key.size()];
		}

		return output;
	}

	size_t get_shift_amount() const
	{
		return shift_amount_;
	}

};

#endif
