#include "ChildProcess.h"
#include "Signal.h"
#include <csignal>
#include <signal.h>

bool ChildProcess::Start() {
	if (this->Initialize() == false) {
		return false;
	}

	Signal::Instance().Add(SIGTERM, [this](int signalValue) { this->Stop(); });

	this->condition.store(true);

	return this->Job();
}

bool ChildProcess::Stop() {
	this->condition.store(false);

	if (this->Finalize() == false) {
		return false;
	}

	return true;
}
