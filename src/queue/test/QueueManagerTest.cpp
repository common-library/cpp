#include "../QueueManager.h"
#include "gtest/gtest.h"
#include <future>
#include <queue>
#include <string>
#include <thread>
#include <vector>

using namespace std;

class TestClass : public ::testing::Test {
	private:
	protected:
		virtual void SetUp() { QueueManager::Instance().Clear(); }

		virtual void TearDown() { QueueManager::Instance().Clear(); }
};

TEST_F(TestClass, Front) {
	QueueManager::Instance().Push<int>("test", 111);
	EXPECT_EQ(QueueManager::Instance().Front<int>("test"), 111);

	QueueManager::Instance().Push<int>("test", 222);
	EXPECT_EQ(QueueManager::Instance().Front<int>("test"), 111);
}

TEST_F(TestClass, Back) {
	QueueManager::Instance().Push<int>("test", 111);
	EXPECT_EQ(QueueManager::Instance().Back<int>("test"), 111);

	QueueManager::Instance().Push<int>("test", 222);
	EXPECT_EQ(QueueManager::Instance().Back<int>("test"), 222);
}

TEST_F(TestClass, Push) {
	QueueManager::Instance().Push<int>("test", 111);
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 1);
	EXPECT_EQ(QueueManager::Instance().Front<int>("test"), 111);
}

TEST_F(TestClass, Emplace) {
	class Test {
		public:
			int i;
			double d;
			string s;

			Test(int i, double d, string s) : i(i), d(d), s(s){};
	};

	const int i = 1;
	const double d = 1.1;
	const string s = "a";

	auto check = [&i, &d, &s](auto t) {
		EXPECT_EQ(t.i, i);
		EXPECT_DOUBLE_EQ(t.d, d);
		EXPECT_STREQ(t.s.c_str(), s.c_str());
	};

	check(QueueManager::Instance().Emplace<Test>("test", i, d, s));
	check(QueueManager::Instance().Front<Test>("test"));
}

TEST_F(TestClass, Pop) {
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 0);

	QueueManager::Instance().Pop<int>("test");
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 0);

	QueueManager::Instance().Push<int>("test", 111);
	QueueManager::Instance().Push<int>("test", 222);

	QueueManager::Instance().Pop<int>("test");
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 1);

	QueueManager::Instance().Pop<int>("test");
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 0);
}

TEST_F(TestClass, Swap) {
	queue<int> swap;
	swap.push(111);
	swap.push(222);

	QueueManager::Instance().Push<int>("test", 333);
	QueueManager::Instance().Push<int>("test", 444);
	QueueManager::Instance().Push<int>("test", 555);

	QueueManager::Instance().Swap<int>("test", swap);

	EXPECT_EQ(swap.size(), 3);
	EXPECT_EQ(swap.front(), 333);
	EXPECT_EQ(swap.back(), 555);

	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 2);
	EXPECT_EQ(QueueManager::Instance().Front<int>("test"), 111);
	EXPECT_EQ(QueueManager::Instance().Back<int>("test"), 222);
}

TEST_F(TestClass, Size) {
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 0);

	QueueManager::Instance().Push<int>("test", 111);
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 1);

	QueueManager::Instance().Push<int>("test", 222);
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 2);

	QueueManager::Instance().Pop<int>("test");
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 1);

	QueueManager::Instance().Pop<int>("test");
	EXPECT_EQ(QueueManager::Instance().Size<int>("test"), 0);
}

TEST_F(TestClass, Empty) {
	EXPECT_TRUE(QueueManager::Instance().Empty<int>("test"));

	QueueManager::Instance().Push<int>("test", 111);
	EXPECT_FALSE(QueueManager::Instance().Empty<int>("test"));
}

TEST_F(TestClass, Clear) {
	QueueManager::Instance().Push<int>("test1", 111);
	QueueManager::Instance().Push<int>("test2", 111);
	QueueManager::Instance().Clear("test1");
	EXPECT_TRUE(QueueManager::Instance().Empty<int>("test1"));
	EXPECT_FALSE(QueueManager::Instance().Empty<int>("test2"));

	QueueManager::Instance().Push<int>("test1", 111);
	QueueManager::Instance().Push<int>("test2", 111);
	QueueManager::Instance().Clear();
	EXPECT_TRUE(QueueManager::Instance().Empty<int>("test1"));
	EXPECT_TRUE(QueueManager::Instance().Empty<int>("test2"));
}

TEST_F(TestClass, Instance) {
	for (int i = 0; i < 1000; i++) {
		EXPECT_EQ(&QueueManager::Instance(), &QueueManager::Instance());
	}
}

TEST_F(TestClass, thread) {
	const int count = 1000;

	vector<future<void>> v = {};
	for (int i = 0; i < count; ++i) {
		v.push_back(async(launch::async, [i]() {
			QueueManager::Instance().Push<int>("test1", i);
			QueueManager::Instance().Push<int>("test2", i);
			QueueManager::Instance().Push<int>("test3", i);
		}));
	}

	for (auto &iter : v) {
		iter.wait();
	}

	EXPECT_EQ(QueueManager::Instance().Size<int>("test1"), count);
	EXPECT_EQ(QueueManager::Instance().Size<int>("test2"), count);
	EXPECT_EQ(QueueManager::Instance().Size<int>("test3"), count);
}
