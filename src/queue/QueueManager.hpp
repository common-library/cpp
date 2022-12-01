template <typename T> void QueueManager::Push(const E_QUEUE_TYPE& eType, const T& data) {
	lock_guard<mutex> lock(*this->mapMutex.at(eType));

	this->mapQueue[eType].push(data);
}

template <typename T> T QueueManager::Front(const E_QUEUE_TYPE& eType) {
	return boost::get<T>(this->mapQueue[eType].front());
}
