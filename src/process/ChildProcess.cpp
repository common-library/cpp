#include <csignal>
#include <cstring>
using namespace std;

#include "FileLog.h"
#include "CommonConfig.h"
#include "EnvironmentVariable.h"

#include "ChildProcess.h"

ChildProcess::ChildProcess()
	: Process(E_PROCESS_TYPE::CHILD)
{
	DEBUG_G(__PRETTY_FUNCTION__);
}

bool ChildProcess::Initialize()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	Singleton<FileLog>::Instance().SetThread(true);

	this->SetSignal();

	Singleton<EnvironmentVariable>::Instance().SetProcess(this->GetProcessType(), this);

	if(Singleton<EnvironmentVariable>::Instance().InitializeLog() == false) {
		ERROR_L_G("InitializeLog fail");
		return false;
	}

	if(this->InitializeDerived() == false) {
		ERROR_L_G("InitializeDerived fail");
		return false;
	}

	Singleton<EnvironmentVariable>::Instance().SetCondition(true);

	return true;
}

bool ChildProcess::Finalize()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	this->FinalizeDerived();

	Singleton<FileLog>::Instance().SetThread(false);

	Singleton<EnvironmentVariable>::Instance().SetProcess(this->GetProcessType(), nullptr);

	this->bCondition.store(false);

	Singleton<EnvironmentVariable>::Instance().SetCondition(false);

	return true;
}

bool ChildProcess::Start()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("child process start");

	if(this->Initialize() == false) {
		ERROR_L_G("Initialize fail");
		return false;
	}

	this->bCondition.store(true);
	while(this->bCondition) {
		this->Job();
	}

	return true;
}

bool ChildProcess::Stop()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("child process stop");

	return this->Finalize();
}

void ChildProcess::SetSignal()
{
	DEBUG_G(__PRETTY_FUNCTION__);

	if(Singleton<EnvironmentVariable>::Instance().GetStandAlone()) {
		sigset(SIGHUP,  SIG_IGN);
		sigset(SIGPIPE, SIG_IGN);
		sigset(SIGCHLD, SIG_IGN);
		sigset(SIGURG,  SIG_IGN);
	}

	sigset(SIGTERM, this->SigTerm);
}

void ChildProcess::SigTerm(int iSig)
{
	DEBUG_G(__PRETTY_FUNCTION__);

	INFO_G("child caught signal %s", strsignal(iSig));

	ChildProcess *pChildProcess = (ChildProcess *)Singleton<EnvironmentVariable>::Instance().GetProcess(E_PROCESS_TYPE::CHILD);
	if(pChildProcess) {
		pChildProcess->Stop();
	}
}
