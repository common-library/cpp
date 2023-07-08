#pragma once

#include <algorithm>
#include <any>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <tuple>

using namespace std;

class QueueManager {
	private:
		QueueManager() = default;
		~QueueManager() = default;

		mutex mutexForKey;

		map<string, unique_ptr<mutex>> mutexByKey;
		map<string, unique_ptr<any>> queueByKey;

		template <typename T>
		tuple<mutex *, queue<T> *> GetQueue(const string &key);

	public:
		template <typename T> T &Front(const string &key);

		template <typename T> T &Back(const string &key);

		template <typename T> void Push(const string &key, auto data);

		template <typename T, typename... Args>
		auto Emplace(const string &key, Args &&...args);

		template <typename T> void Pop(const string &key);

		template <typename T> void Swap(const string &key, queue<T> &swap);

		template <typename T> int Size(const string &key);

		template <typename T> bool Empty(const string &key);

		void Clear();
		void Clear(const string &key);

		static QueueManager &Instance();
};

template <typename T>
tuple<mutex *, queue<T> *> QueueManager::GetQueue(const string &key) {
	lock_guard<mutex> lock(this->mutexForKey);

	if (this->mutexByKey.find(key) == this->mutexByKey.end()) {
		this->mutexByKey[key] = make_unique<mutex>();
	}

	if (this->queueByKey.find(key) == this->queueByKey.end()) {
		this->queueByKey[key] = make_unique<any>(make_any<queue<T>>());
	}

	return make_tuple(this->mutexByKey[key].get(),
					  any_cast<queue<T>>(this->queueByKey[key].get()));
}

template <typename T> T &QueueManager::Front(const string &key) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->front();
}

template <typename T> T &QueueManager::Back(const string &key) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->back();
}

template <typename T> void QueueManager::Push(const string &key, auto data) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->push(data);
}

template <typename T, typename... Args>
auto QueueManager::Emplace(const string &key, Args &&...args) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->emplace(args...);
}

template <typename T> void QueueManager::Pop(const string &key) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	if (get<1>(info)->size()) {
		get<1>(info)->pop();
	}
}

template <typename T>
void QueueManager::Swap(const string &key, queue<T> &swap) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	get<1>(info)->swap(swap);
}

template <typename T> int QueueManager::Size(const string &key) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->size();
}

template <typename T> bool QueueManager::Empty(const string &key) {
	auto info = this->GetQueue<T>(key);

	lock_guard<mutex> lock(*get<0>(info));

	return get<1>(info)->empty();
}
