#include "CommonConfig.h"
#include <string>

using namespace std;

CommonConfig::CommonConfig()
	: Config("common.config"), logLevel(LOG_LEVEL::DEBUG), workingPath(""),
	  logOutputPath(""), logFileName(""), logLinePrint(false),
	  logThreadMode(false) {}

bool CommonConfig::InitializeDerived() {
	this->logLevel = LOG_LEVEL::DEBUG;
	for (const auto &iter : LOG_LEVEL_INFO) {
		if (this->json->GetValue<string>({"log_level"}) == iter.second) {
			this->logLevel = iter.first;
			break;
		}
	}

	this->logOutputPath = this->json->GetValue<string>({"log_output_path"});
	this->workingPath = this->json->GetValue<string>({"working_path"});
	this->logFileName = this->json->GetValue<string>({"log_file_name"});
	this->logLinePrint = this->json->GetValue<bool>({"log_line_print"});
	this->logThreadMode = this->json->GetValue<bool>({"log_thread_mode"});

	return true;
}

LOG_LEVEL CommonConfig::GetLogLevel() const { return this->logLevel; }

string CommonConfig::GetWorkingPath() const { return this->workingPath; }

string CommonConfig::GetLogOutputPath() const { return this->logOutputPath; }

string CommonConfig::GetLogFileName() const { return this->logFileName; }

bool CommonConfig::GetLogLinePrint() const { return this->logLinePrint; }

bool CommonConfig::GetLogThreadMode() const { return this->logThreadMode; }
