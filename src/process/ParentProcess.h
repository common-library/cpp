#pragma once

#include "ChildProcess.h"
#include "Process.h"
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

class ParentProcess : public Process {
	private:
		int pidFD;
		bool standAlone;
		string binaryName;
		atomic_bool stop;
		atomic<pid_t> pid;

		mutex mutexForInfos;
		mutex mutexForHandler;

		vector<unique_ptr<ChildProcess>> processes;
		map<pid_t, unique_ptr<ChildProcess>> infos;

		bool Initialize() override final;
		bool InitializeSignal();
		bool Finalize() override final;

		bool StartChildProcess();
		bool StopChildProcess();

		bool MakeDaemon();

		bool LockPidFile();
		bool UnLockPidFile();

		void SignalHandlerChild(int signalValue);
		void SignalHandlerTerm(int signalValue);

	public:
		ParentProcess(const bool &standAlone, const string &binaryName,
					  vector<unique_ptr<ChildProcess>> &processes);
		~ParentProcess();

		bool IsParentProcess() const;

		bool Start() override final;
		bool Stop() override final;
};
