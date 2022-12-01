#pragma once

#include <sys/types.h>

#include <atomic>
#include <string>
#include <vector>
using namespace std;

#include "Process.h"

class EnvironmentVariable {
	private:
		gid_t gid;
		uid_t uid;

		pid_t parentPid;
		pid_t childPid;

		bool bStandAlone;

		string strConfigPath;
		string strProcessName;

		atomic_bool bCondition;

		vector<string> vecArgv;

		map<E_PROCESS_TYPE, Process*> mapProcess;

		bool InitializeOptions(int iArgc, char* pcArgv[]);
		bool InitializeCurrentPath(const string& strWorkingPath);

	public:
		EnvironmentVariable();
		virtual ~EnvironmentVariable() = default;

		bool Initialize(int iArgc, char* pcArgv[]);
		bool InitializeLog();

		string Usage();

		bool GetStandAlone();
		string GetConfigPath();
		string GetProcessName();

		gid_t GetGid();
		void SetGid(const gid_t& gid);

		uid_t GetUid();
		void SetUid(const uid_t& uid);

		pid_t GetParentPid();
		void SetParentPid(const pid_t& pid);

		pid_t GetChildPid();
		void SetChildPid(const pid_t& pid);

		bool GetCondition();
		void SetCondition(const bool& bCondition);

		Process* GetProcess(const E_PROCESS_TYPE& eProcessType);
		void SetProcess(const E_PROCESS_TYPE& eProcessType, Process* pProcess);
};
