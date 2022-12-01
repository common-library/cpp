#include "RabbitJson.h"

rabbit::object RabbitJson::GetObject(const vector<string>& vecKey) {
	rabbit::object object = this->document[vecKey[0]];

	for (unsigned int ui = 1; ui < vecKey.size(); ui++) {
		object = object[vecKey[ui]];
	}

	return object;
}

bool RabbitJson::Parsing(const string& strContents) {
	this->document.parse(strContents.c_str());

	return true;
}

JSON_VALUE_TYPE RabbitJson::GetValue(const vector<string>& vecKey,
									 const JSON_VALUE_TYPE& valueType) {
	rabbit::object object = this->GetObject(vecKey);

	if (valueType.type() == typeid(bool)) {
		return object.as_bool();
	} else if (valueType.type() == typeid(int)) {
		return object.as_int();
	} else if (valueType.type() == typeid(double)) {
		return object.as_double();
	} else if (valueType.type() == typeid(string)) {
		return object.str();
	}

	return boost::blank();
}

vector<map<string, JSON_VALUE_TYPE>>
RabbitJson::GetArray(const vector<string>& vecKey, const string& strArrayName,
					 const map<string, JSON_VALUE_TYPE>& mapValueInfo) {

	vector<string> vecFinalKeyTemp = vecKey;
	vecFinalKeyTemp.push_back(strArrayName);

	const vector<string> vecFinalKey = vecFinalKeyTemp;

	rabbit::object object = this->GetObject(vecFinalKey);

	vector<map<string, JSON_VALUE_TYPE>> vecValue;
	vecValue.clear();

	for (uint32_t i = 0; i < object.size(); i++) {
		map<string, JSON_VALUE_TYPE> mapValue;
		mapValue.clear();

		for (const auto& iter2 : mapValueInfo) {
			const rabbit::object o_value =
				iter2.first.size() ? object[i][iter2.first] : object[i];

			if (iter2.second.type() == typeid(bool)) {
				mapValue[iter2.first] = o_value.as_bool();
			} else if (iter2.second.type() == typeid(int)) {
				mapValue[iter2.first] = o_value.as_int();
			} else if (iter2.second.type() == typeid(double)) {
				mapValue[iter2.first] = o_value.as_double();
			} else if (iter2.second.type() == typeid(string)) {
				mapValue[iter2.first] = o_value.str();
			}
		}

		if (mapValue.empty()) {
			continue;
		}

		vecValue.push_back(mapValue);
	}

	return vecValue;
}
