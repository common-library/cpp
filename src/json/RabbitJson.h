#pragma once

#include "rabbit.hpp"

#include "Json.h"

class RabbitJson : public Json {
private:
	rabbit::document document;
public:
	RabbitJson() = default;
	virtual ~RabbitJson() = default;

	rabbit::object GetObject(const vector<string> &vecKey);

	virtual bool Parsing(const string &strContents);

	virtual JSON_VALUE_TYPE GetValue(const vector<string> &vecKey, const JSON_VALUE_TYPE &valueType);

	virtual vector<map<string, JSON_VALUE_TYPE>> GetArray(const vector<string> &vecKey, const string &strArrayName, const map<string, JSON_VALUE_TYPE> &mapValueInfo);
};
