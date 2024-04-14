#include "Signal.h"
#include <atomic>
#include <csignal>
#include <functional>
#include <mutex>

using namespace std;

mutex Signal::mutexForInitialize;
atomic_bool Signal::initialize = false;

Signal::Signal() : pid(getpid()) {}

bool Signal::CheckPid() { return this->pid == getpid(); }

void Signal::Add(const int &signalValue, function<void(int)> handler) {
	{
		lock_guard<mutex> lock(this->mutexForInfos);

		this->infos[signalValue] = handler;
	}

	this->SetSignal(signalValue, Signal::Handler);
}

void Signal::Delete() {
	lock_guard<mutex> lock(this->mutexForInfos);

	for (const auto &info : this->infos) {
		this->SetSignal(info.first, SIG_DFL);
	}
}

void Signal::Delete(const int &signalValue) {
	this->SetSignal(signalValue, SIG_DFL);

	{
		lock_guard<mutex> lock(this->mutexForInfos);

		if (this->infos.find(signalValue) != this->infos.end()) {
			this->infos.erase(signalValue);
		}
	}
}

void Signal::SetSignal(const int &signalValue, __sighandler_t handler) {
	signal(signalValue, handler);
}

void Signal::Handler(int signalValue) {
	if (Signal::initialize) {
		return;
	}

	function<void(int)> handler = nullptr;

	{
		lock_guard<mutex> lock(Signal::Instance().mutexForInfos);

		if (Signal::Instance().infos.find(signalValue) != Signal::Instance().infos.end()) {
			handler = Signal::Instance().infos.at(signalValue);
		}
	}

	if (handler != nullptr) {
		handler(signalValue);
	}
}

Signal &Signal::Instance() {
	static Signal signal;

	if (signal.CheckPid()) {
		return signal;
	}

	{
		lock_guard<mutex> lock(Signal::mutexForInitialize);

		if (signal.CheckPid()) {
			return signal;
		}

		Signal::initialize.store(true);

		signal.Delete();

		signal.pid.store(getpid());

		Signal::initialize.store(false);
	}

	return signal;
}
