#pragma once

#include <boost/property_tree/json_parser.hpp>

#include "Json.h"

class BoostJson : public Json {
private:
	boost::property_tree::ptree ptree;
public:
	BoostJson() = default;
	virtual ~BoostJson() = default;

	string MakeKey(const vector<string> &vecKey) const;

	virtual bool Parsing(const string &strContents);

	virtual JSON_VALUE_TYPE GetValue(const vector<string> &vecKey, const JSON_VALUE_TYPE &valueType);

	virtual vector<map<string, JSON_VALUE_TYPE>> GetArray(const vector<string> &vecKey, const string &strArrayName, const map<string, JSON_VALUE_TYPE> &mapValueInfo);
};
