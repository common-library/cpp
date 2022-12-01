#include "RapidjsonJson.h"

rapidjson::Value& RapidjsonJson::GetTopValue(const vector<string>& vecKey) {
	rapidjson::Value& value = this->document[vecKey[0].c_str()];

	for (unsigned int ui = 1; ui < vecKey.size(); ui++) {
		value = value[vecKey[ui].c_str()];
	}

	this->document.CopyFrom(this->documentOrg, this->document.GetAllocator());

	return value;
}

bool RapidjsonJson::Parsing(const string& strContents) {
	this->document.Parse(strContents.c_str());

	if (this->document.HasParseError()) {
		return false;
	}

	this->documentOrg.CopyFrom(this->document, this->documentOrg.GetAllocator());

	return true;
}

JSON_VALUE_TYPE RapidjsonJson::GetValue(const vector<string>& vecKey,
										const JSON_VALUE_TYPE& valueType) {
	rapidjson::Value& value = this->GetTopValue(vecKey);

	JSON_VALUE_TYPE result;

	if (valueType.type() == typeid(bool)) {
		return value.GetBool();
	} else if (valueType.type() == typeid(int)) {
		return value.GetInt();
	} else if (valueType.type() == typeid(double)) {
		return value.GetDouble();
	} else if (valueType.type() == typeid(string)) {
		return string(value.GetString());
	}

	return boost::blank();
}

vector<map<string, JSON_VALUE_TYPE>>
RapidjsonJson::GetArray(const vector<string>& vecKey, const string& strArrayName,
						const map<string, JSON_VALUE_TYPE>& mapValueInfo) {
	vector<string> vecFinalKeyTemp = vecKey;
	vecFinalKeyTemp.push_back(strArrayName);

	const vector<string> vecFinalKey = vecFinalKeyTemp;

	rapidjson::Value& v_top = this->GetTopValue(vecFinalKey);

	vector<map<string, JSON_VALUE_TYPE>> vecValue;
	vecValue.clear();

	for (const auto& iter : v_top.GetArray()) {
		map<string, JSON_VALUE_TYPE> mapValue;
		mapValue.clear();

		for (const auto& iter2 : mapValueInfo) {
			const rapidjson::Value& value =
				iter2.first.size() ? iter[iter2.first.c_str()] : iter;

			if (iter2.second.type() == typeid(bool)) {
				mapValue[iter2.first] = value.GetBool();
			} else if (iter2.second.type() == typeid(int)) {
				mapValue[iter2.first] = int(value.GetInt());
			} else if (iter2.second.type() == typeid(double)) {
				mapValue[iter2.first] = value.GetDouble();
			} else if (iter2.second.type() == typeid(string)) {
				mapValue[iter2.first] = string(value.GetString());
			}
		}

		if (mapValue.empty()) {
			continue;
		}

		vecValue.push_back(mapValue);
	}

	return vecValue;
}
