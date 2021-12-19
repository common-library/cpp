#pragma once

#include <atomic>
using namespace std;

#include "Process.h"

class ChildProcess : public Process {
private:
	atomic_bool bCondition;

	bool Initialize();
	bool Finalize();

	void SetSignal();

	static void SigTerm(int iSig);
protected:
	virtual bool InitializeDerived() = 0;
	virtual bool FinalizeDerived() = 0;
	virtual bool Job() = 0;
public:
	ChildProcess();
	virtual ~ChildProcess() = default;

	virtual bool Start();
	virtual bool Stop();
};
