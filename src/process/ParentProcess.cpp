#include "ParentProcess.h"
#include "FileManager.h"
#include "Signal.h"
#include <csignal>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <vector>

using namespace std;

ParentProcess::ParentProcess(const bool &standAlone, const string &binaryName,
							 vector<unique_ptr<ChildProcess>> &processes)
	: pidFD(-1), standAlone(standAlone), binaryName(binaryName), stop(false), pid(getpid()) {
	for (auto &process : processes) {
		this->processes.push_back(move(process));
	}
}

ParentProcess::~ParentProcess() { this->Stop(); }

bool ParentProcess::Initialize() {
	this->stop.store(false);

	if (this->MakeDaemon() == false) {
		return false;
	}

	if (this->LockPidFile() == false) {
		return false;
	}

	if (this->InitializeSignal() == false) {
		return false;
	}

	return true;
}

bool ParentProcess::InitializeSignal() {
	Signal::Instance().Add(SIGHUP, SIG_IGN);
	Signal::Instance().Add(SIGPIPE, SIG_IGN);
	Signal::Instance().Add(SIGPIPE, SIG_IGN);
	Signal::Instance().Add(SIGURG, SIG_IGN);

	Signal::Instance().Add(SIGCHLD,
						   [this](int signalValue) { this->SignalHandlerChild(signalValue); });

	Signal::Instance().Add(SIGTERM,
						   [this](int signalValue) { this->SignalHandlerTerm(signalValue); });

	if (this->standAlone) {
		Signal::Instance().Add(SIGINT, SIG_IGN);
	} else {
		Signal::Instance().Add(SIGINT,
							   [this](int signalValue) { this->SignalHandlerTerm(signalValue); });
	}

	return true;
}

bool ParentProcess::Finalize() {
	if (this->IsParentProcess() == false) {
		lock_guard<mutex> lock(this->mutexForInfos);

		this->infos.clear();

		return true;
	}

	if (this->StopChildProcess() == false) {
		return false;
	}

	if (this->UnLockPidFile() == false) {
		return false;
	}

	return true;
}

bool ParentProcess::LockPidFile() {
	const string path = FileManager::Instance().GetCurrentPath() + "/" + this->binaryName + ".pid";

	this->pidFD = FileManager::Instance().LockBetweenProcess(path);
	if (this->pidFD == -1) {
		return false;
	}

	if (ftruncate(this->pidFD, 0) == -1) {
		return false;
	}

	const auto pid = to_string(getpid());
	if (write(this->pidFD, pid.c_str(), pid.size()) != (ssize_t)pid.size()) {
		return false;
	}

	return true;
}

bool ParentProcess::UnLockPidFile() {
	if (this->pidFD == -1) {
		return true;
	}

	FileManager::Instance().UnLockBetweenProcess(this->pidFD);
	if (close(this->pidFD) == -1) {
		return false;
	}
	this->pidFD = -1;

	return true;
}

bool ParentProcess::MakeDaemon() {
	if (this->standAlone == false) {
		return true;
	}

	const int pid1 = fork();
	if (pid1 == -1) {
		return false;
	} else if (pid1 != 0) {
		exit(0);
	}

	setsid();

	const int pid2 = fork();
	if (pid2 == -1) {
		return false;
	} else if (pid2 != 0) {
		exit(0);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	this->pid = getpid();

	return true;
}

void ParentProcess::SignalHandlerTerm(int signalValue) { this->Stop(); }

void ParentProcess::SignalHandlerChild(int signalValue) {
	lock_guard<mutex> lock(this->mutexForHandler);

	int status = 0;
	int oldPid = -1;
	while ((oldPid = wait3(&status, WNOHANG, (struct rusage *)0)) > 0) {
		if (WIFEXITED(status)) {
			{
				lock_guard<mutex> lock(this->mutexForInfos);

				this->infos.erase(oldPid);

				if (this->infos.empty()) {
					this->Stop();
					return;
				}
			}

			continue;
		}

		if (this->stop) {
			continue;
		}

		const int newPid = fork();
		if (newPid == -1) {
			exit(-1);
		} else if (newPid == 0) {
			Signal::Instance().Add(SIGINT, SIG_IGN);

			unique_ptr<ChildProcess> process = nullptr;

			{
				lock_guard<mutex> lock(this->mutexForInfos);

				if (this->infos.find(oldPid) != this->infos.end()) {
					process = move(this->infos.at(oldPid));
				}
			}

			if (process) {
				process->Start();
			}

			this->Stop();
		} else {
			lock_guard<mutex> lock(this->mutexForInfos);

			this->infos[newPid] = move(this->infos.at(oldPid));
			this->infos.erase(oldPid);
		}
	}
}

bool ParentProcess::IsParentProcess() const { return this->pid == getpid(); }

bool ParentProcess::Start() {
	if (this->Initialize() == false) {
		return false;
	}

	if (this->StartChildProcess() == false) {
		return false;
	}

	this->condition.store(true);
	while (this->stop == false) {
		this_thread::sleep_for(1ms);
	}
	this->condition.store(false);

	return this->Finalize();
}

bool ParentProcess::StartChildProcess() {
	if (this->StopChildProcess() == false) {
		return false;
	}

	for (auto &process : this->processes) {
		const int pid = fork();
		if (pid == -1) {
			exit(-1);
		} else if (pid == 0) {
			Signal::Instance().Add(SIGINT, SIG_IGN);

			return move(process)->Start() && this->Stop();
		} else {
			lock_guard<mutex> lock(this->mutexForInfos);
			this->infos[pid] = move(process);
		}
	}
	processes.clear();

	return true;
}

bool ParentProcess::Stop() {
	this->stop.store(true);

	return true;
}

bool ParentProcess::StopChildProcess() {
	while (true) {
		{
			lock_guard<mutex> lock(this->mutexForHandler);
		}

		vector<int> pids = {};

		{
			lock_guard<mutex> lock(this->mutexForInfos);

			if (this->infos.empty()) {
				return true;
			}

			for (const auto &info : this->infos) {
				struct stat sts;
				const string command = "/proc/" + to_string(info.first);
				if (stat(command.c_str(), &sts) == -1 && errno == ENOENT) {
					this->infos.erase(info.first);
					break;
				}

				pids.push_back(info.first);
			}
		}

		for (const auto &pid : pids) {
			if (this->IsParentProcess() == false) {
				return true;
			}

			kill(pid, SIGTERM);
		}

		this_thread::sleep_for(100ms);
	}

	return true;
}
