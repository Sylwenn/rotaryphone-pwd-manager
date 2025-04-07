#pragma once
#ifndef FILE_MANAGER
#define FILE_MANAGER
class file_manager {
public:
	static bool does_file_exist(const std::string& filename) {
		const std::ifstream file(filename);
		return file.good();
	}

	static void create_file(const std::string& filename) {
		std::ofstream file;
		file.open(filename);
		file.close();
	}

	static void delete_file(const std::string& filename) {
		remove(filename.c_str());
	}

	static void write_to_file(const std::string& filename, const std::string& data) {
		std::ofstream file;

		if (is_binary(data)) {
			file.open(filename, std::ios::binary);
		}
		else {
			file.open(filename);
		}

		if (!file) {
			std::cerr << "Error opening file for writing!" << "\n";
			return;
		}

		file.write(data.c_str(), data.size());
		file.close();
	}

	static void write_line_by_line_to_file(const std::string& filename, const std::vector<std::string>& lines) {
		std::ofstream file;
		if (is_binary(lines[0])) { //Binary Check
			file.open(filename, std::ios::binary);
		}
		else {
			file.open(filename);
		}

		if (!file) { //File Check
			std::cerr << "Error opening file for writing!" << "\n";
			return;
		}

		for (const std::string& line : lines) { //Add to new files
			file.write(line.c_str(), line.size());
			file.put('\n');
		}

		file.close();
	}

	static void write_to_next_line(const std::string& filename, const std::string& data) {
		std::ofstream file;
		if (is_binary(data)) {
			file.open(filename, std::ios::binary | std::ios::app);
		}
		else {
			file.open(filename, std::ios::app);
		}

		if (!file) {
			std::cerr << "Error opening file for writing!" << "\n";
			return;
		}

		file.write(data.c_str(), data.size());
		file.put('\n');

		file.close();
	}

	static std::string read_from_file(const std::string& filename) {
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file) {
			std::cerr << "Error opening file for reading!" << "\n";
			return "";
		}
		const size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::string content(size, '\0');
		file.read(content.data(), size);
		file.close();

		return content;
	}

	static std::vector<std::string> read_from_file_line_by_line(const std::string& filename) {
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

	static void delete_specific_line(const std::string& filename, int line_to_delete) {
		std::vector<std::string> lines;
		std::ifstream file(filename);
		std::string line;
		int current_line = 0;
		while (std::getline(file, line)) {
			if (current_line != line_to_delete) {
				lines.push_back(line);
			}
			current_line++;
		}
		file.close();

		if (line_to_delete < 0 || line_to_delete >= static_cast<int>(lines.size())) {
			std::cerr << "Error: Invalid line number!" << "\n";
			return;
		}

		std::ofstream out_file(filename);
		if (!out_file) {
			std::cerr << "Error opening file for writing!" << "\n";
			return;
		}

		for (const auto& l : lines) {
			out_file << l << "\n";
		}
		out_file.close();

		std::cout << "Line " << line_to_delete << " deleted successfully." << "\n";
	}
};

#endif