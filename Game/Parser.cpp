#include "Precompiled.h"

#include "Parser.h"
#include "Game.h"

Parser::Parser() {
	mParsed = false;
}

void Parser::ParseFile(string fn) {
	FILE * file = 0;

	fopen_s(&file, fn.c_str(), "r");

	if (!file) {
		throw std::runtime_error(StringBuilder("Unable to parse file ") << fn);
		return;
	}

	mFileName = fn;

	string str;

	while (!feof(file)) {
		char symbol = '\0';
		fread(&symbol, sizeof(char), 1, file);
		str.push_back(symbol);
	};

	fclose(file);

	if (str.size() <= 1) {
		throw std::runtime_error(StringBuilder("Unable to parse file ") << fn);
		return;
	}

	mValueList.clear();

	bool equalFound = false;
	bool quoteLF = false, quoteRF = false;
	unsigned int n = 0;

	string varName, var;

	while (true) {
		char symbol = str.at(n);

		if (symbol == ';') {
			quoteLF = false;
			quoteRF = false;
			equalFound = false;

			varName.clear();
			var.clear();
		};

		if (isalpha((unsigned char)symbol) || isdigit((unsigned char)symbol) || symbol == '_') {
			if (!equalFound) {
				varName.push_back(symbol);
			}
		} else {
			if (symbol == '=') {
				equalFound = true;
			}

			if (symbol == '"') {
				if (quoteLF == false) {
					quoteLF = true;
				} else {
					quoteRF = true;
				}
			}
		};

		if (quoteLF) {
			if (quoteRF) {
				mValueList[varName] = var;
			} else if (symbol != '"') {
				var.push_back(symbol);
			}
		};

		n++;

		if (n >= str.size()) {
			break;
		}
	};

	mParsed = true;
}

bool Parser::Empty() {
	return mValueList.empty();
}

float Parser::GetNumber(string varName) {
	auto iValue = mValueList.find(varName);

	if (iValue != mValueList.end()) {
		return atof(iValue->second.c_str());
	} else {
		throw std::runtime_error(StringBuilder("Unable to get number '") << varName << "' from " << mFileName << "!");
	}

	// never reached
	return 0.0f;
}

const string & Parser::GetString(string varName) {
	auto iValue = mValueList.find(varName);

	if (iValue != mValueList.end()) {
		return iValue->second;
	}
	throw std::runtime_error(StringBuilder("Unable to get string '") << varName << "' from " << mFileName << "!");
}

bool Parser::IsParsed() {
	return mParsed;
}
