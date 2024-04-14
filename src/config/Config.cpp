#include "Config.h"
#include "JsonFactory.h"
#include <filesystem>
#include <string>

using namespace std;

Config::Config(const string &fileName)
	: fileName(fileName), json(JsonFactory::Instance().Make(JSON_TYPE::RAPIDJSON)) {}

bool Config::Initialize(const filesystem::path &path) {
	const filesystem::path fullPath = path.string() + "/" + this->fileName;
	if (this->json->ParsingFromFile(fullPath) == false) {
		return false;
	}

	return this->InitializeDerived();
}

string Config::GetFileName() const { return this->fileName; }
