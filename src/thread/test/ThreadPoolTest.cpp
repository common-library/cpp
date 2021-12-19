#include "../ThreadPool.h"

#include "gtest/gtest.h"

TEST(ThreadPoolTest, AddJob)
{
	const int iPoolSize = 1024;

	ThreadPool threadPool(iPoolSize);

	auto job = [](int i)->int{cout << i << endl; sleep(3); return i * 10;};

	map<int, future<int>> mapResult;
	mapResult.clear();
	for(int i = 0 ; i < iPoolSize ; i++) {
		mapResult[i] = threadPool.AddJob(job, i);
	}

	for(auto &iter : mapResult) {
		EXPECT_EQ(iter.first * 10, iter.second.get());
	}
}

TEST(ThreadPoolTest, GetPoolSize)
{
	const int iPoolSize = 10;

	ThreadPool threadPool(iPoolSize);

	EXPECT_EQ(threadPool.GetPoolSize(), iPoolSize);
}
