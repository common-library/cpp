#pragma once

#include <memory>
using namespace std;

#include "ChildProcess.h"

class ParentProcess : public Process {
private:
	int iPidFileFD;

	unique_ptr<ChildProcess> uniqptrChildProcess;

	bool Initialize();
	bool InitializePidFile();

	bool Finalize();
	bool FinalizePidFile();
	bool FinalizeChildProcess();

	bool MakeDaemon();

	bool Job();

	void SetSignal();

	static void SigChild(int iSig);
	static void SigTerm(int iSig);
public:
	ParentProcess(unique_ptr<ChildProcess> uniqptrChildProcess);
	virtual ~ParentProcess() = default;

	virtual bool Start();
	virtual bool Stop();
};
