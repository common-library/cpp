#include "ThreadPool.h"
#include <functional>
#include <future>
#include <mutex>

using namespace std;

ThreadPool::ThreadPool(const poolSizeType &poolSize)
	: stop(false), poolSize(poolSize), wakeUpAdmin(true) {
	this->futureAdmin =
		async(launch::async, mem_fn(&ThreadPool::AdminWorker), this);
}

ThreadPool::~ThreadPool() {
	{
		unique_lock<mutex> lock1(this->mutexAdmin);
		unique_lock<mutex> lock2(this->mutexUser);
		this->stop = true;
	}

	this->SetPoolSize(0);
	this->cvAdmin.notify_one();
	this->cvUser.notify_all();

	this->futureAdmin.get();
};

void ThreadPool::AdminWorker() {
	while (true) {
		unique_lock<mutex> lock(this->mutexAdmin);

		this->cvAdmin.wait(
			lock, [this]() { return this->wakeUpAdmin || this->stop; });

		if (this->stop && this->runningUser == 0) {
			break;
		}

		for (auto iter = this->futureUser.begin();
			 iter != this->futureUser.end();) {

			switch (iter->wait_for(1ns)) {
			case future_status::ready:
				iter->get();
				iter = this->futureUser.erase(iter);

				break;
			default:
				++iter;

				break;
			}
		}

		for (poolSizeType index = this->futureUser.size();
			 index < this->poolSize; ++index) {
			this->futureUser.push_back(
				async(launch::async, mem_fn(&ThreadPool::UserWorker), this));
		}

		if (this->futureUser.size() == this->poolSize) {
			this->wakeUpAdmin = false;
		} else {
			this->AddJob([]() {});
			this->cvUser.notify_all();
		}
	}
}

void ThreadPool::UserWorker() {
	++this->runningUser;

	while (true) {
		unique_lock<mutex> lock(this->mutexUser);

		this->cvUser.wait(
			lock, [this]() { return this->stop || this->queueUserJob.size(); });

		if (this->stop && this->queueUserJob.empty()) {
			break;
		}

		if (this->stop == false && this->poolSize < this->runningUser) {
			break;
		}

		auto job = this->queueUserJob.front();
		this->queueUserJob.pop();
		lock.unlock();

		++this->runningJob;
		job();
		--this->runningJob;
	}

	--this->runningUser;
}

size_t ThreadPool::GetWaitingJobSize() const {
	lock_guard<mutex> lock(this->mutexUser);

	return this->queueUserJob.size();
}

poolSizeType ThreadPool::GetRunningJobSize() const { return this->runningJob; }

poolSizeType ThreadPool::GetPoolSize() const { return this->poolSize; }

poolSizeType ThreadPool::GetCurrentPoolSize() const {
	lock_guard<mutex> lock(this->mutexAdmin);

	return this->futureUser.size();
}

void ThreadPool::SetPoolSize(const poolSizeType &poolSize, const bool &async) {
	this->poolSize = poolSize;

	{
		lock_guard<mutex> lock(this->mutexAdmin);
		this->wakeUpAdmin = true;
	}

	this->cvAdmin.notify_one();

	if (async == false) {
		while (this->poolSize != this->GetCurrentPoolSize()) {
		}
	}
}
