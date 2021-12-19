#pragma once

#include <map>
#include <condition_variable>
using namespace std;

enum class E_PROCESS_TYPE {
	PARENT = 0,
	CHILD,
};

static const map<E_PROCESS_TYPE, string> GmapProcessInfo = {
		{E_PROCESS_TYPE::PARENT, "PARENT"},
		{E_PROCESS_TYPE::CHILD, "CHILD"},
	};

class Process {
private:
	const E_PROCESS_TYPE eProcessType;
protected:

public:
	Process(const E_PROCESS_TYPE &eProcessType) : eProcessType(eProcessType) {};
	virtual ~Process() = default;

	virtual bool Start() = 0;
	virtual bool Stop() = 0;

	E_PROCESS_TYPE GetProcessType() {return this->eProcessType;}
};
