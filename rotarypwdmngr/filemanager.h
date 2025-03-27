#pragma once
#include "includes.h"


bool isBinary(const std::string& data)
{
	for (char ch : data) {
		if (ch < 32 || ch > 126) {
			return true;
		}
	}
	return false; 
}

class FileManager
{
public:
	void createFile(std::string filename)
	{
		std::ofstream file;
		file.open(filename);
		file.close();
	}
	void deleteFile(std::string filename)
	{
		remove(filename.c_str());
	}
	void writeToFile(const std::string& filename, const std::string& data)
	{
		std::ofstream file;

		if (isBinary(data)) {
			file.open(filename, std::ios::binary);
		}
		else {
			file.open(filename);
		}

		if (!file) {
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		file.write(data.c_str(), data.size());
		file.close();
	}
	std::string readFromFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file) {
			std::cerr << "Error opening file for reading!" << std::endl;
			return "";
		}
		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::string content(size, '\0');
		file.read(&content[0], size);
		file.close();

		if (isBinary(content)) {
			std::cout << "The file is binary." << std::endl;
		}
		else {
			std::cout << "The file is text." << std::endl;
		}

		return content;
	}
	std::vector<std::string> readFromFileLineByLine(std::string filename)
	{
		std::ifstream file;
		file.open(filename);
		std::vector<std::string> data;
		std::string line;
		while (std::getline(file, line))
		{
			data.push_back(line);
		}
		file.close();
		return data;
	}
};