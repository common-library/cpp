#include "EnvironmentVariable.h"
#include "CommonConfig.h"
#include "FileManager.h"
#include <cstring>
#include <string>
#include <vector>

using namespace std;

bool EnvironmentVariable::Initialize(int argc, char *argv[]) {
	if (argc == 0) {
		return true;
	}

	vector<string> args{};
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	if (this->Initialize(args) == false) {
		return false;
	}

	return true;
}

bool EnvironmentVariable::Initialize(const vector<string> &args) {
	extern int optind;
	optind = 1;

	if (args.empty()) {
		return true;
	}

	if (this->InitializeOptions(args) == false) {
		return false;
	}

	if (this->InitializeCurrentPath() == false) {
		return false;
	}

	return true;
}

bool EnvironmentVariable::InitializeOptions(const vector<string> &args) {
	vector<char *> argsToChar;
	argsToChar.reserve(args.size());

	for (auto &iter : args) {
		argsToChar.push_back(const_cast<char *>(iter.c_str()));
	}

	this->standAlone = false;
	this->configPath = "";
	this->binaryName = args[0].substr(args[0].find_last_of('/') + 1);

	int opt = 0;
	while ((opt = getopt(argsToChar.size(), argsToChar.data(), "c:s")) != -1) {
		switch (opt) {
		case 'c':
			if (optarg == nullptr || strlen(optarg) == 0) {
				return false;
			}

			this->configPath = FileManager::Instance().ToAbsolutePath(optarg);

			break;
		case 's':
			this->standAlone = true;

			break;
		default:
			return false;
		}
	}

	return true;
}

bool EnvironmentVariable::InitializeCurrentPath() const {
	CommonConfig commonConfig;
	if (commonConfig.Initialize(this->configPath) == false) {
		return false;
	}

	const string workingPath = commonConfig.GetWorkingPath();

	if (workingPath.empty()) {
		return true;
	}

	if (FileManager::Instance().MakeDirs(workingPath) == false) {
		return false;
	}

	if (FileManager::Instance().SetCurrentPath(workingPath) == false) {
		return false;
	}

	return true;
}

string EnvironmentVariable::Usage() const {
	return "usage: " + this->binaryName + " -c config_path [-s]";
}

bool EnvironmentVariable::GetStandAlone() const { return this->standAlone; }
string EnvironmentVariable::GetConfigPath() const { return this->configPath; }
string EnvironmentVariable::GetBinaryName() const { return this->binaryName; }
