#include "Json.h"
#include "FileManager.h"
#include <filesystem>
#include <string>
#include <vector>

using namespace std;

bool Json::ParsingFromFile(const filesystem::path &path) const {
	const auto [data, errorCode] = FileManager::Instance().Read(path);
	if (errorCode) {
		return false;
	}

	return this->ParsingFromString(data);
}

bool Json::WhetherTheKeyExists(const vector<string> &key) const {
	return this->GetObject(key).has_value();
}
