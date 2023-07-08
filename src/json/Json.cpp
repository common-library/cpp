#include "Json.h"
#include <string>
#include <vector>

using namespace std;

bool Json::ParsingFromFile(const string &path) const {
	const auto result = FileManager::Instance().Read(path);
	if (get<0>(result) == false) {
		return false;
	}

	return this->ParsingFromString(get<1>(result));
}

bool Json::WhetherTheKeyExists(const vector<string> &key) const {
	return this->GetObject(key).has_value();
}
