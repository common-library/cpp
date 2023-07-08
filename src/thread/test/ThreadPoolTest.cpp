#include "../ThreadPool.h"
#include "gtest/gtest.h"
#include <atomic>
#include <future>
#include <random>
#include <thread>
#include <vector>

using namespace std;

TEST(ThreadPoolTest, AddJob) {
	const int poolSize = 10;

	ThreadPool threadPool(poolSize);

	auto func = [&threadPool]() {
		vector<future<int>> futures{};

		const int jobSize = 100;
		for (int i = 0; i < jobSize; ++i) {
			futures.push_back(threadPool.AddJob([](auto i) { return i; }, i));
		}

		for (int i = 0; i < jobSize; ++i) {
			EXPECT_EQ(futures.at(i).get(), i);
		}
	};

	vector<future<void>> futures{};
	for (int i = 0; i < 100; ++i) {
		futures.push_back(async(launch::async, func));
	}

	for (auto &iter : futures) {
		iter.get();
	}
}

TEST(ThreadPoolTest, GetWaitingJobSize) {
	const int poolSize = 1;

	ThreadPool threadPool(poolSize);

	atomic_bool stop = false;
	atomic_int count = 0;
	vector<future<void>> futures{};
	auto job = [&stop, &count]() {
		++count;
		while (stop == false) {
			this_thread::sleep_for(1ns);
		}
	};

	for (int i = 0; i < 100; ++i) {
		futures.push_back(threadPool.AddJob(job));
		while (count == 0) {
			this_thread::sleep_for(1ns);
		}
		EXPECT_EQ(threadPool.GetWaitingJobSize(), i);
	}

	stop = true;

	for (auto &iter : futures) {
		iter.get();
	}
}

TEST(ThreadPoolTest, GetRunningJobSize) {
	const int poolSize = 10;

	ThreadPool threadPool(poolSize);

	atomic_bool stop = false;
	atomic_int count = 0;
	vector<future<void>> futures{};
	auto job = [&stop, &count]() {
		++count;
		while (stop == false) {
			this_thread::sleep_for(1ns);
		}
	};

	for (int i = 1; i <= poolSize; ++i) {
		futures.push_back(threadPool.AddJob(job));
		while (count != i) {
			this_thread::sleep_for(1ns);
		}
		EXPECT_EQ(threadPool.GetRunningJobSize(), i);
	}

	for (int i = 1; i <= poolSize; ++i) {
		futures.push_back(threadPool.AddJob(job));
		EXPECT_EQ(threadPool.GetRunningJobSize(), poolSize);
	}

	stop = true;

	for (auto &iter : futures) {
		iter.get();
	}
}

TEST(ThreadPoolTest, GetPoolSize) {
	const int poolSize = 1;

	ThreadPool threadPool(poolSize);

	EXPECT_EQ(threadPool.GetPoolSize(), poolSize);

	for (int i = 0; i < 100; ++i) {
		threadPool.SetPoolSize(i);
		EXPECT_EQ(threadPool.GetPoolSize(), i);
	}
}

TEST(ThreadPoolTest, GetCurrentPoolSize) {
	const int poolSize = 1;

	ThreadPool threadPool(poolSize);

	EXPECT_EQ(threadPool.GetPoolSize(), poolSize);

	for (int i = 0; i < 10; ++i) {
		threadPool.SetPoolSize(i, false);
		EXPECT_EQ(threadPool.GetCurrentPoolSize(), i);
	}
}

TEST(ThreadPoolTest, SetPoolSize) {
	const int poolSize = 1;

	ThreadPool threadPool(poolSize);

	auto get_random = [](auto start, auto end) {
		default_random_engine generator(random_device{}());
		uniform_int_distribution<int> distribution(start, end);

		return distribution(generator);
	};

	for (int i = 0; i < 10; ++i) {
		threadPool.SetPoolSize(get_random(0, 100), false);
		EXPECT_EQ(threadPool.GetPoolSize(), threadPool.GetCurrentPoolSize());
	}

	vector<future<void>> futures{};
	for (int i = 0; i < 10; ++i) {
		futures.push_back(async(launch::async, [&threadPool, &get_random]() {
			threadPool.SetPoolSize(get_random(0, 100));
		}));
	}
	for (auto &iter : futures) {
		iter.get();
	}
	while (threadPool.GetPoolSize() != threadPool.GetCurrentPoolSize()) {
	}
}
