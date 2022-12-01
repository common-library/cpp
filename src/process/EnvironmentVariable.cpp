#include <getopt.h>

#include <cstring>

#include "CommonConfig.h"
#include "FileLog.h"
#include "FileManager.h"

#include "EnvironmentVariable.h"

EnvironmentVariable::EnvironmentVariable()
	: gid(-1), uid(-1), parentPid(-1), childPid(-1), bStandAlone(false),
	  strConfigPath(""), strProcessName(""), bCondition(false), vecArgv({}),
	  mapProcess({}) {
	DEBUG_G(__PRETTY_FUNCTION__);
}

bool EnvironmentVariable::Initialize(int iArgc, char* pcArgv[]) {
	DEBUG_G(__PRETTY_FUNCTION__);

	if (this->InitializeOptions(iArgc, pcArgv) == false) {
		ERROR_L_G("InitializeOptions fail");
		return false;
	}

	CommonConfig commonConfig;
	if (commonConfig.Initialize(this->strConfigPath) == false) {
		ERROR_L_G("CommonConfig Initialize fail");
		return false;
	}

	if (this->InitializeCurrentPath(commonConfig.GetWorkingPath()) == false) {
		ERROR_L_G("InitializeCurrentPath fail");
		return false;
	}

	return true;
}

bool EnvironmentVariable::InitializeLog() {
	DEBUG_G(__PRETTY_FUNCTION__);

	CommonConfig commonConfig;
	if (commonConfig.Initialize(this->strConfigPath) == false) {
		ERROR_L_G("CommonConfig Initialize fail");
		return false;
	}

	if (Singleton<FileLog>::Instance().Initialize(
			commonConfig.GetLogLevel(), commonConfig.GetLogOutputPath(),
			commonConfig.GetLogFileNamePrefix(), commonConfig.GetThreadMode()) == false) {
		ERROR_L_G("FileLog Initialize fail");
		return false;
	}

	return true;
}

bool EnvironmentVariable::InitializeOptions(int iArgc, char* pcArgv[]) {
	DEBUG_G(__PRETTY_FUNCTION__);

	if (iArgc == 0 || pcArgv == nullptr) {
		ERROR_L_G("invalid argument");
		return false;
	}

	this->bStandAlone = false;
	this->strConfigPath = "";

	char* pcStr = strrchr(pcArgv[0], '/');
	this->strProcessName = pcStr ? pcStr + 1 : pcArgv[0];

	int iOpt = 0;
	while ((iOpt = getopt(iArgc, pcArgv, "c:s")) != -1) {
		switch (iOpt) {
		case 'c':
			if (optarg && optarg[0]) {
				this->strConfigPath = FileManager().ToAbsolutePath(optarg);
			} else {
				return false;
			}

			break;
		case 's':
			this->bStandAlone = true;

			break;
		default:
			return false;
		}
	}

	for (int i = 0; i < iArgc; i++) {
		this->vecArgv.push_back(pcArgv[0]);
	}

	return true;
}

bool EnvironmentVariable::InitializeCurrentPath(const string& strWorkingPath) {
	DEBUG_G(__PRETTY_FUNCTION__);

	if (strWorkingPath.empty()) {
		return true;
	}

	if (FileManager().MakeDirs(strWorkingPath) == false) {
		ERROR_L_G("MakeDirs fail - path : (%s), error : (%s)", strWorkingPath.c_str(),
				  strerror(errno));

		return false;
	}

	if (FileManager().SetCurrentPath(strWorkingPath) == false) {
		ERROR_L_G("SetCurrentPath fail - path : (%s), error : (%s)",
				  strWorkingPath.c_str(), strerror(errno));

		return false;
	}

	return true;
}

string EnvironmentVariable::Usage() {
	DEBUG_G(__PRETTY_FUNCTION__);

	char caBuffer[4096];
	snprintf(caBuffer, sizeof(caBuffer), "usage: %s -c config_path [-s]",
			 this->strProcessName.c_str());

	return caBuffer;
}

bool EnvironmentVariable::GetStandAlone() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->bStandAlone;
}

string EnvironmentVariable::GetConfigPath() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->strConfigPath;
}

string EnvironmentVariable::GetProcessName() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->strProcessName;
}

gid_t EnvironmentVariable::GetGid() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->gid;
}

void EnvironmentVariable::SetGid(const gid_t& gid) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->gid = gid;
}

uid_t EnvironmentVariable::GetUid() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->uid;
}

void EnvironmentVariable::SetUid(const uid_t& uid) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->uid = uid;
}

pid_t EnvironmentVariable::GetParentPid() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->parentPid;
}

void EnvironmentVariable::SetParentPid(const pid_t& pid) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->parentPid = pid;
}

pid_t EnvironmentVariable::GetChildPid() {
	DEBUG_G(__PRETTY_FUNCTION__);

	return this->childPid;
}

void EnvironmentVariable::SetChildPid(const pid_t& pid) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->childPid = pid;
}

bool EnvironmentVariable::GetCondition() { return this->bCondition; }

void EnvironmentVariable::SetCondition(const bool& bCondition) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->bCondition.store(bCondition);
}

Process* EnvironmentVariable::GetProcess(const E_PROCESS_TYPE& eProcessType) {
	DEBUG_G(__PRETTY_FUNCTION__);

	if (this->mapProcess.find(eProcessType) != this->mapProcess.end()) {
		return this->mapProcess.at(eProcessType);
	}

	return nullptr;
}

void EnvironmentVariable::SetProcess(const E_PROCESS_TYPE& eProcessType,
									 Process* pProcess) {
	DEBUG_G(__PRETTY_FUNCTION__);

	this->mapProcess[eProcessType] = pProcess;
}
