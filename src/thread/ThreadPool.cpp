#include "ThreadPool.h"

ThreadPool::ThreadPool(const size_t &poolSize)
	: bStop(false)
{
	this->vecPool.reserve(poolSize);
	for(size_t i = 0 ; i < this->vecPool.capacity() ; i++) {
		this->vecPool.emplace_back([this]() {this->Worker();});
	}
}

ThreadPool::~ThreadPool()
{
	this->bStop.store(true);

	this->cvJob.notify_all();

	for(auto &iter : this->vecPool) {
		iter.join();
	}
}

void ThreadPool::Worker()
{
	while(true) {
		const auto job = this->GetJob();
		if(job == nullptr) {
			break;
		}

		job();
	}
}

function<void()> ThreadPool::GetJob()
{
	unique_lock<mutex> lock(this->mutexJob);

	this->cvJob.wait(lock, [this](){return this->queueJob.size() || this->bStop;});

	if(this->bStop && this->queueJob.empty()) {
		return nullptr;
	}

	auto job = this->queueJob.front();
	this->queueJob.pop();

	lock.unlock();

	return job;
}

void ThreadPool::SetJob(function<void()> job)
{
	lock_guard<mutex> lock(this->mutexJob);

	this->queueJob.push([job](){(job)();});
}

size_t ThreadPool::GetPoolSize()
{
	return this->vecPool.capacity();
}
