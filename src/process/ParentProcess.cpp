#include <unistd.h>
#include <sys/wait.h>

#include <csignal>
#include <cstring>
using namespace std;

#include "FileLog.h"
#include "Singleton.h"
#include "FileManager.h"
#include "EnvironmentVariable.h"

#include "ParentProcess.h"

ParentProcess::ParentProcess(unique_ptr<ChildProcess> uniqptrChildProcess)
	: Process(E_PROCESS_TYPE::PARENT),
		iPidFileFD(-1), uniqptrChildProcess(move(uniqptrChildProcess))
{
	DEBUG_G(__PRETTY_FUNCTION__);
}

bool ParentProcess::Initialize()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	this->Finalize();

	if(this->MakeDaemon() == false) {
		ERROR_L_G("MakeProcessType error");
		return false;
	}

	if(this->InitializePidFile() == false) {
		ERROR_L_G("WritePid error");

		this->FinalizePidFile();

		return false;
	}

	Singleton<EnvironmentVariable>::Instance().SetGid(getgid());
	Singleton<EnvironmentVariable>::Instance().SetUid(getuid());

	this->SetSignal();

	Singleton<EnvironmentVariable>::Instance().SetProcess(this->GetProcessType(), this);

	return true;
}

bool ParentProcess::InitializePidFile()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	const string strProcessName = Singleton<EnvironmentVariable>::Instance().GetProcessName();
	const string strPath = FileManager().GetCurrentPath() + "/" + strProcessName + ".pid";

/*
	const pid_t parentPid = Singleton<EnvironmentVariable>::Instance().GetParentPid();
	if(FileManager().Write(strPath, to_string(parentPid), ios::trunc) == false) {
		ERROR_L_G("Write fail - path : (%s), error : (%s)", strPath.c_str(), strerror(errno));
		return false;
	}
*/

	this->iPidFileFD = FileManager().LockBetweenProcess(strPath);
	if(this->iPidFileFD == -1) {
		if(errno == EAGAIN) {
			ERROR_L_G("another process is already running - path : (%s)", strPath.c_str());
		} else {
			ERROR_L_G("LockBetweenProcess fail - path : (%s), error : (%s)", strPath.c_str(), strerror(errno));
		}

		return false;
	}

	if(ftruncate(this->iPidFileFD, 0) == -1) {
		ERROR_L_G("ftruncate fail - file : (%s), error : (%s)", strPath.c_str(), strerror(errno));

		return false;
	}

	const pid_t parentPid = Singleton<EnvironmentVariable>::Instance().GetParentPid();
	const string strParentPid = to_string(parentPid);
	if(write(this->iPidFileFD, strParentPid.c_str(), strParentPid.size()) != (ssize_t)strParentPid.size()) {
		ERROR_L_G("write fail - file : (%s), error : (%s)", strPath.c_str(), strerror(errno));

		return false;
	}

	return true;
}

bool ParentProcess::Finalize()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	this->FinalizeChildProcess();

	this->FinalizePidFile();

	Singleton<EnvironmentVariable>::Instance().SetProcess(this->GetProcessType(), nullptr);

	Singleton<EnvironmentVariable>::Instance().SetCondition(false);

	return true;
}

bool ParentProcess::FinalizePidFile()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	if(this->iPidFileFD != -1) {
		FileManager().UnLockBetweenProcess(this->iPidFileFD);
		close(this->iPidFileFD);
		this->iPidFileFD = -1;
	}

	return true;
}

bool ParentProcess::FinalizeChildProcess()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	sigset(SIGCHLD, SIG_IGN);

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		const pid_t childPid = Singleton<EnvironmentVariable>::Instance().GetChildPid();
		if(childPid != -1) {
			return kill(childPid, SIGTERM) == 0 ? true : false;
		}
	} else {
		if(this->uniqptrChildProcess.get() != nullptr) {
			return this->uniqptrChildProcess->Stop();
		}
	}

	return true;
}

bool ParentProcess::MakeDaemon()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		const int iPid1 = fork();
		if(iPid1 == -1) {
			ERROR_L_G("fork fail - error : (%s)", strerror(errno));
			return false;
		} else if(iPid1 != 0) {
			exit(0);
		}

		setsid();
		sigset(SIGHUP, SIG_IGN);

		const int iPid2 = fork();
		if(iPid2 == -1) {
			ERROR_L_G("fork fail - error : (%s)", strerror(errno));
			return false;
		} else if(iPid2 != 0) {
			exit(0);
		}
	}

	Singleton<EnvironmentVariable>::Instance().SetParentPid(getpid());

	return true;
}

bool ParentProcess::Job()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		this->iPidFileFD = -1;
	}

	if(this->uniqptrChildProcess.get() == nullptr) {
		INFO_G("child process is nullptr");
		return true;
	}

	return this->uniqptrChildProcess->Start();
}

bool ParentProcess::Start()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("parent process start");

	if(this->Initialize() == false) {
		ERROR_L_G("Initialize fail");
		return false;
	}

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		const int iPid = fork();

		if(iPid == -1) {
			ERROR_L_G("fork fail - error : (%s)", strerror(errno));
			return false;
		} else if(iPid == 0) {
			return this->Job();
		} else {
			Singleton<EnvironmentVariable>::Instance().SetChildPid(iPid);
			Singleton<EnvironmentVariable>::Instance().SetCondition(true);

			while(Singleton<EnvironmentVariable>::Instance().GetCondition()) {
				pause();
			}

			return true;
		}	 
	} else {
		return this->Job();
	}

	return false;
}

bool ParentProcess::Stop()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("parent process stop");

	return this->Finalize();
}

void ParentProcess::SetSignal()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		sigset(SIGHUP, SIG_IGN);
		sigset(SIGPIPE, SIG_IGN);
		sigset(SIGURG, SIG_IGN);
		sigset(SIGINT, SIG_IGN);

		sigset(SIGCHLD, this->SigChild);
	} else {
		sigset(SIGCHLD, SIG_IGN);
	}

	sigset(SIGTERM, this->SigTerm);
}

void ParentProcess::SigChild(int iSig)
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("parent caught signal %s", strsignal(iSig));

	Singleton<EnvironmentVariable>::Instance().SetChildPid(-1);

	int iWstat = 0;
	int iOldPid = -1;
	while((iOldPid = wait3(&iWstat, WNOHANG, (struct rusage *) 0)) > 0) {
		if(WIFEXITED(iWstat) == 1) {
			INFO_G("child process normal stop, so parent process stop");
			Singleton<EnvironmentVariable>::Instance().SetCondition(false);
			break;
		}

		const int iNewPid = fork();
		if(iNewPid == -1) {
			ERROR_L_G("fork fail - error : (%s)", strerror(errno));
			exit(-1);
		} else if(iNewPid == 0) {
			ParentProcess *pParentProcess = (ParentProcess *)Singleton<EnvironmentVariable>::Instance().GetProcess(E_PROCESS_TYPE::PARENT);
			if(pParentProcess) {
				if(pParentProcess->Job() == false) {
					ERROR_L_G("parent process run error so exit");
					exit(-1);
				}
			} else {
				ERROR_L_G("parent process is nullptr");
				exit(-1);
			}
		} else {
			Singleton<EnvironmentVariable>::Instance().SetChildPid(iNewPid);
		}
	}
}

void ParentProcess::SigTerm(int iSig)
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("parent caught signal %s", strsignal(iSig));

	ParentProcess *pParentProcess = (ParentProcess *)Singleton<EnvironmentVariable>::Instance().GetProcess(E_PROCESS_TYPE::PARENT);
	if(pParentProcess == nullptr) {
		return;
	}

	if(pParentProcess->Stop() == false) {
		ERROR_L_G("ParentProcess Stop fail");
		return;
	}
}
