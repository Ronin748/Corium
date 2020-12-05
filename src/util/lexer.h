#ifndef CRMLEXER_H
#define CRMLEXER_H

#include <corium.h>

bool expr(std::vector<std::string> *words, int i, int l, std::string cmd)
{
	if (words->size() > i) {
		if (words->at(i) == cmd) {
			if (words->size() >= i + l) {
				return true;
			} else {
				return false;
			}
		}
	}
	return false;
}

bool expr(std::vector<std::string> words, int i, int l, std::string cmd)
{
	if (words.size() > i) {
		if (words[i] == cmd) {
			if (words.size() >= i + l) {
				return true;
			} else {
				return false;
			}
		}
	}
	return false;
}

bool wordIs(std::vector<std::string> words, int i, std::string word)
{
	if (words.size() > i) {
		if (words[i] == word) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

bool wordIsNot(std::vector<std::string> words, int i, std::string word)
{
	if (words.size() > i) {
		if (words[i] != word) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

bool boolParser(std::string input)
{
	bool output = false;

	if (input == "true" || input == "_TRUE") output = true;

	return output;
}

float parseAdd(std::string input)
{
	float sum = 0.0f;

	std::string val = "";

	for (int i = 0; i < input.length(); ++i) {
		if (input[i] == '+') {
			sum += (float) atof((val).c_str());
			++i;
			val = "";
		} else {
			val += input[i];
		}
	}
	sum += (float) atof((val).c_str());

	return sum;
}

glm::vec3 vecParser(std::string input)
{
	glm::vec3 vector = glm::vec3(0.0f);

	std::string buffer = "";
	int k = 0;
	for (int i = 0; i < input.length(); ++i) {
		if (input[i] == ',') {
			vector[k] = parseAdd(buffer);
			++k;
			buffer = "";
		} else if (input[i] != '(' && input[i] != ')') {
			buffer += input[i];
		}
	}

	vector[k] = parseAdd(buffer);

	return vector;
}

std::vector<std::string> fileLexer(std::string path)
{
	std::ifstream file;

	file.open(path);

	std::vector<std::string> words = {""};

	if (file.is_open()) {
		char read = ' ';

		const char WHITE_SPACE(32);

		bool is_word = false;

		int last = 0;

		for (int i = 0; !file.eof(); ++i) {
			//file >> read;
			read = file.get();

			if (read == '#') {
				bool is_comment = true;
				while (!file.eof() && is_comment) {
					read = file.get();
					if (read == '#' || read == ';' || read == '\n' || read == '\r') {
						is_comment = false;
					}
				}
			}

			if (read == '\"' || read == '(' || read == ')') {
				is_word = !is_word;
				read = file.get();
			}

			if ((read != WHITE_SPACE || is_word) && (read != ';' && read != '\n' && read != '\r' && read != '\t')) {
				words[words.size() - 1] += read;
				last = 0;
			} else if (last < 1) {
				++last;
				words.resize(words.size() + 1);
				words[words.size() - 1] = "";
			}
		}
	} else {
		gamePrint(path + " not found!\n");
	}

	file.close();

	return words;
}

#endif
