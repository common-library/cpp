#pragma once

#include <map>
using namespace std;

#include "Json.h"

enum class E_JSON_PARSER {
	BOOST,
	RABBIT,
	RAPIDJSON,
};

class JsonManager {
private:
	const E_JSON_PARSER eJsonParser;

	const map<E_JSON_PARSER, Json *> mapJson;
public:
	JsonManager(const E_JSON_PARSER &eJsonParser);
	virtual ~JsonManager();

	bool ParsingFile(const string &strPath) const;
	bool ParsingString(const string &strContents) const;

	template <class T>
	T GetValue(const vector<string> &vecKey) const {
		return boost::get<T>(this->mapJson.at(this->eJsonParser)->GetValue(vecKey, T()));
	}

	vector<map<string, JSON_VALUE_TYPE>> GetArray(const vector<string> &vecKey, const string &strArrayName, const map<string, JSON_VALUE_TYPE> &mapValueInfo) const;
};
