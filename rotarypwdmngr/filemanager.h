#pragma once
#ifndef FILE_MANAGER
#define FILE_MANAGER
#include "includes.h"
class FileManager {
public:
	bool doesFileExist(const std::string& filename)
	{
		std::ifstream file(filename);
		return file.good();
	}
	void createFile(std::string filename) {
		std::ofstream file;
		file.open(filename);
		file.close();
	}
	void deleteFile(std::string filename) {
		remove(filename.c_str());
	}
	void writeToFile(const std::string& filename, const std::string& data) {
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
	void writeLineByLineToFile(const std::string& filename, const std::vector<std::string>& lines) {
		std::ofstream file;
		if (isBinary(lines[0])) { //Binary Check
			file.open(filename, std::ios::binary);
		}
		else {
			file.open(filename);
		}

		if (!file) { //File Check
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		for (const std::string& line : lines) { //Add to new files
			file.write(line.c_str(), line.size()); 
			file.put('\n');
		}

		file.close();
	}

	void writeToNextLine(const std::string& filename, const std::string& data) {
		std::ofstream file;
		if (isBinary(data)) {
			file.open(filename, std::ios::binary | std::ios::app);  
		}
		else {
			file.open(filename, std::ios::app);
		}

		if (!file) {
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		file.write(data.c_str(), data.size()); 
		file.put('\n'); 

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
			//std::cout << "The file is binary." << std::endl;
		} else {
			//std::cout << "The file is text." << std::endl;
		}

		return content;
	}
	std::vector<std::string> readFromFileLineByLine(std::string filename) {
		std::ifstream file;
		file.open(filename);
		std::vector<std::string> data;
		std::string line;
		while (std::getline(file, line)) {
			data.push_back(line);
		}
		file.close();
		return data;
	}
};

#endif