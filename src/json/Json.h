#pragma once

#include <map>
#include <string>
#include <vector>
using namespace std;

#include "boost/variant.hpp"

using JSON_VALUE_TYPE = boost::variant<boost::blank, bool, int, double, string>;

class Json {
	private:
	public:
		Json() = default;
		virtual ~Json() = default;

		virtual bool Parsing(const string& strContents) = 0;

		virtual JSON_VALUE_TYPE GetValue(const vector<string>& vecKey,
										 const JSON_VALUE_TYPE& valueType) = 0;

		virtual vector<map<string, JSON_VALUE_TYPE>>
		GetArray(const vector<string>& vecKey, const string& strArrayName,
				 const map<string, JSON_VALUE_TYPE>& mapValueInfo) = 0;
};
