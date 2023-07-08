#pragma once

#include "Config.h"
#include "FileLog.h"
#include <string>

using namespace std;

class CommonConfig : public Config {
	private:
		LOG_LEVEL logLevel;
		string workingPath;
		string logOutputPath;
		string logFileName;
		bool logLinePrint;
		bool logThreadMode;

		virtual bool InitializeDerived() final;

	public:
		CommonConfig();
		virtual ~CommonConfig() = default;

		LOG_LEVEL GetLogLevel() const;
		string GetWorkingPath() const;
		string GetLogOutputPath() const;
		string GetLogFileName() const;
		bool GetLogLinePrint() const;
		bool GetLogThreadMode() const;
};
