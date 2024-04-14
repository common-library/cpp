#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

using namespace std;

using poolSizeType = size_t;

class ThreadPool {
	private:
		bool stop;

		atomic<poolSizeType> poolSize;

		mutable mutex mutexAdmin;
		bool wakeUpAdmin;
		future<void> futureAdmin;
		condition_variable cvAdmin;

		mutable mutex mutexUser;
		vector<future<void>> futureUser;
		atomic<poolSizeType> runningUser;
		atomic<poolSizeType> runningJob;
		queue<function<void()>> queueUserJob;
		condition_variable cvUser;

		void AdminWorker();
		void UserWorker();

	public:
		ThreadPool(const poolSizeType &poolSize);
		~ThreadPool();

		template <class Func, class... Args>
		future<typename result_of<Func(Args...)>::type> AddJob(Func &&func, Args &&...args);

		size_t GetWaitingJobSize() const;
		poolSizeType GetRunningJobSize() const;

		poolSizeType GetPoolSize() const;
		poolSizeType GetCurrentPoolSize() const;
		void SetPoolSize(const poolSizeType &poolSize, const bool &async = true);
};

template <class Func, class... Args>
future<typename result_of<Func(Args...)>::type> ThreadPool::AddJob(Func &&func, Args &&...args) {
	using returnType = typename result_of<Func(Args...)>::type;

	auto job =
		make_shared<packaged_task<returnType()>>(bind(forward<Func>(func), forward<Args>(args)...));

	if (this->stop) {
		(*job)();
	} else {
		{
			lock_guard<mutex> lock(this->mutexUser);

			this->queueUserJob.push([job]() { (*job)(); });
		}

		this->cvUser.notify_one();
	}

	return job->get_future();
}
