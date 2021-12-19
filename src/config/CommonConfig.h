#pragma once

#include "FileLog.h"

#include "Config.h"

class CommonConfig : public Config {
private:
	string strWorkingPath;

	E_LOG_LEVEL eLogLevel;
	bool bThreadMode;
	string strLogOutputPath;
	string strLogFileNamePrefix;

	virtual bool InitializeDerived();
public:
	CommonConfig();
	virtual ~CommonConfig() = default;

	string GetWorkingPath();

	E_LOG_LEVEL GetLogLevel();
	bool GetThreadMode();
	string GetLogOutputPath();
	string GetLogFileNamePrefix();
};
