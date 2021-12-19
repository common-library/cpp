#include "BoostJson.h"

string BoostJson::MakeKey(const vector<string> &vecKey) const
{
	if(vecKey.empty()) {
		return "";
	}

	string strKey = vecKey[0];
	for(unsigned int ui = 1 ; ui < vecKey.size() ; ui++) {
		strKey += "." + vecKey[ui];
	}

	return strKey;
}

bool BoostJson::Parsing(const string &strContents)
{
	stringstream ss;
	ss << strContents;

	boost::property_tree::read_json(ss, this->ptree);

	return true;
}

JSON_VALUE_TYPE BoostJson::GetValue(const vector<string> &vecKey, const JSON_VALUE_TYPE &valueType)
{
	const string strKey = this->MakeKey(vecKey);

	if(valueType.type() == typeid(bool)) {
		return this->ptree.get<bool>(strKey);
	} else if(valueType.type() == typeid(int)) {
		return this->ptree.get<int>(strKey);
	} else if(valueType.type() == typeid(double)) {
		return this->ptree.get<double>(strKey);
	} else if(valueType.type() == typeid(string)) {
		return this->ptree.get<string>(strKey);
	}

	return boost::blank();
}

vector<map<string, JSON_VALUE_TYPE>> BoostJson::GetArray(const vector<string> &vecKey, const string &strArrayName, const map<string, JSON_VALUE_TYPE> &mapValueInfo)
{
	const string strKey = this->MakeKey(vecKey);

	const string strFinalKey = strKey.size() ? strKey + "." + strArrayName : strArrayName;

	vector<map<string, JSON_VALUE_TYPE>> vecValue;
	vecValue.clear();

	for(const auto &iter : this->ptree.get_child(strFinalKey)) {
		map<string, JSON_VALUE_TYPE> mapValue;
		mapValue.clear();

		for(const auto &iter2 : mapValueInfo) {
			if(iter2.second.type() == typeid(bool)) {
				mapValue[iter2.first] = iter.second.get<bool>(iter2.first);
			} else if(iter2.second.type() == typeid(int)) {
				mapValue[iter2.first] = iter.second.get<int>(iter2.first);
			} else if(iter2.second.type() == typeid(double)) {
				mapValue[iter2.first] = iter.second.get<double>(iter2.first);
			} else if(iter2.second.type() == typeid(string)) {
				mapValue[iter2.first] = iter.second.get<string>(iter2.first);
			}
		}

		if(mapValue.empty()) {
			continue;
		}

		vecValue.push_back(mapValue);
	}

	return vecValue;
}
