#pragma once

#include "rapidjson/document.h"

#include "Json.h"

class RapidjsonJson : public Json {
	private:
		rapidjson::Document document;
		rapidjson::Document documentOrg;

	public:
		RapidjsonJson() = default;
		virtual ~RapidjsonJson() = default;

		rapidjson::Value& GetTopValue(const vector<string>& vecKey);

		virtual bool Parsing(const string& strContents);

		virtual JSON_VALUE_TYPE GetValue(const vector<string>& vecKey,
										 const JSON_VALUE_TYPE& valueType);

		virtual vector<map<string, JSON_VALUE_TYPE>>
		GetArray(const vector<string>& vecKey, const string& strArrayName,
				 const map<string, JSON_VALUE_TYPE>& mapValueInfo);
};
