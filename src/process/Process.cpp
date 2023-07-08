#include "Process.h"

Process::Process() : condition(false) {}

bool Process::GetCondition() const { return this->condition; }
