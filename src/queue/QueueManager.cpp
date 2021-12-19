#include <memory>
using namespace std;

#include "QueueManager.h"

QueueManager::QueueManager()
{
	for(const auto &iter : GmapQueueTypeInfo) {
//		this->mapMutex.emplace(iter.first, unique_ptr<mutex>(new mutex()));
		this->mapMutex.emplace(iter.first, make_unique<mutex>());
	}
}

void QueueManager::Pop(const E_QUEUE_TYPE &eType)
{
	lock_guard<mutex> lock(*this->mapMutex.at(eType));

	if(this->mapQueue[eType].empty()) {
		return;
	}

	this->mapQueue[eType].pop();
}

bool QueueManager::Empty(const E_QUEUE_TYPE &eType)
{
	lock_guard<mutex> lock(*this->mapMutex.at(eType));

	return this->mapQueue[eType].empty();
}

int QueueManager::Size(const E_QUEUE_TYPE &eType)
{
	lock_guard<mutex> lock(*this->mapMutex.at(eType));

	return this->mapQueue[eType].size();
}

QueueManager& QueueManager::Instance()
{
	static QueueManager instance;
	return instance;
}
