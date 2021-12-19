#pragma once

#include <queue>
#include <atomic>
#include <future>
#include <thread>
#include <vector>
#include <functional>
using namespace std;

class ThreadPool {
private:
	mutex mutexJob;

	atomic<bool> bStop;

	condition_variable cvJob;

	vector<thread> vecPool;
	queue<function<void()>> queueJob;

	void Worker();

	function<void()> GetJob();
	void SetJob(function<void()> job);
public:
	ThreadPool(const size_t &poolSize);
	virtual ~ThreadPool();

	template <class Func, class... Args>
	future<typename result_of<Func(Args...)>::type> AddJob(Func&& func, Args&&... args);

	size_t GetPoolSize();
};

template <class Func, class... Args>
future<typename result_of<Func(Args...)>::type> ThreadPool::AddJob(Func&& func, Args&&... args)
{
	if(this->bStop) {
		throw logic_error("thread pool is stop");
	}

	using returnType = typename result_of<Func(Args...)>::type;

	auto job = make_shared<packaged_task<returnType()>>(bind(forward<Func>(func), forward<Args>(args)...));

	this->SetJob([job](){(*job)();});

	this->cvJob.notify_one();

	return job->get_future();
}
