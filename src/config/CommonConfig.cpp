#include "CommonConfig.h"

CommonConfig::CommonConfig()
	: Config("common.config"), strWorkingPath(""), eLogLevel(E_LOG_LEVEL::DEBUG),
	  bThreadMode(false), strLogOutputPath(""), strLogFileNamePrefix("") {}

bool CommonConfig::InitializeDerived() {
	this->strWorkingPath = this->jsonManager.GetValue<string>({"working_path"});

	this->eLogLevel = E_LOG_LEVEL::DEBUG;
	const string strLogLevel = this->jsonManager.GetValue<string>({"log_level"});
	for (const auto& iter : GmapLogLevelInfo) {
		if (strLogLevel == iter.second) {
			this->eLogLevel = iter.first;
		}
	}

	this->bThreadMode = this->jsonManager.GetValue<bool>({"thread_mode"});

	this->strLogOutputPath = this->jsonManager.GetValue<string>({"log_output_path"});

	this->strLogFileNamePrefix =
		this->jsonManager.GetValue<string>({"log_file_name_prefix"});

	return true;
}

string CommonConfig::GetWorkingPath() { return this->strWorkingPath; }

E_LOG_LEVEL CommonConfig::GetLogLevel() { return this->eLogLevel; }

bool CommonConfig::GetThreadMode() { return this->bThreadMode; }

string CommonConfig::GetLogOutputPath() { return this->strLogOutputPath; }

string CommonConfig::GetLogFileNamePrefix() { return this->strLogFileNamePrefix; }
