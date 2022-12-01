#include "BoostJson.h"
#include "FileManager.h"
#include "RabbitJson.h"
#include "RapidjsonJson.h"

#include "JsonManager.h"

JsonManager::JsonManager(const E_JSON_PARSER& eJsonParser)
	: eJsonParser(eJsonParser),
	  mapJson({{E_JSON_PARSER::BOOST, new BoostJson()},
			   {E_JSON_PARSER::RABBIT, new RabbitJson()},
			   {E_JSON_PARSER::RAPIDJSON, new RapidjsonJson()}}) {}

JsonManager::~JsonManager() {
	for (const auto& iter : this->mapJson) {
		if (iter.second) {
			delete iter.second;
		}
	}
}

bool JsonManager::ParsingFile(const string& strPath) const {
	string strContents = "";
	if (FileManager().Read(strPath, strContents) == false) {
		return false;
	}

	return this->ParsingString(strContents);
}

bool JsonManager::ParsingString(const string& strContents) const {
	return this->mapJson.at(this->eJsonParser)->Parsing(strContents);
}

vector<map<string, JSON_VALUE_TYPE>>
JsonManager::GetArray(const vector<string>& vecKey, const string& strArrayName,
					  const map<string, JSON_VALUE_TYPE>& mapValueInfo) const {
	return this->mapJson.at(this->eJsonParser)
		->GetArray(vecKey, strArrayName, mapValueInfo);
}
