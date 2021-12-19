#include <cstring>
using namespace std;

#include "Config.h"

Config::Config(const string &strFileName, const E_JSON_PARSER &eJsonParser)
	: strFileName(strFileName),
		jsonManager(eJsonParser)
{
}

bool Config::Initialize(const string &strConfigPath)
{

	const string strPath = strConfigPath + "/" + this->strFileName;
	if(this->jsonManager.ParsingFile(strPath) == false) {
		return false;
	}

	return this->InitializeDerived();
}
