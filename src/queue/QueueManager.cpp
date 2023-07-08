#include "QueueManager.h"
#include <mutex>
#include <string>

using namespace std;

void QueueManager::Clear() {
	lock_guard<mutex> lock(this->mutexForKey);

	this->queueByKey.clear();
	this->mutexByKey.clear();
}

void QueueManager::Clear(const string &key) {
	lock_guard<mutex> lock(this->mutexForKey);

	this->queueByKey.erase(key);
	this->mutexByKey.erase(key);
}

QueueManager &QueueManager::Instance() {
	static QueueManager instance;
	return instance;
};
