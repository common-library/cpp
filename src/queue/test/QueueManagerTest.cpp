#include "../QueueManager.h"

#include "gtest/gtest.h"

class TestClass : public ::testing::Test {
	private:
		void MakeQueueEmpty() {
			while (QueueManager::Instance().Size(E_QUEUE_TYPE::SAMPLE)) {
				QueueManager::Instance().Pop(E_QUEUE_TYPE::SAMPLE);
			}
		}

	protected:
		virtual void SetUp() { this->MakeQueueEmpty(); }

		virtual void TearDown() { this->MakeQueueEmpty(); }
};

TEST_F(TestClass, Push) { QueueManager::Instance().Push(E_QUEUE_TYPE::SAMPLE, 1); }

TEST_F(TestClass, Pop) { QueueManager::Instance().Pop(E_QUEUE_TYPE::SAMPLE); }

TEST_F(TestClass, Front) {
	QueueManager::Instance().Push(E_QUEUE_TYPE::SAMPLE, 123);

	EXPECT_EQ(QueueManager::Instance().Front<int>(E_QUEUE_TYPE::SAMPLE), 123);
}

TEST_F(TestClass, Empty) {
	EXPECT_TRUE(QueueManager::Instance().Empty(E_QUEUE_TYPE::SAMPLE));

	QueueManager::Instance().Push(E_QUEUE_TYPE::SAMPLE, 1);

	EXPECT_FALSE(QueueManager::Instance().Empty(E_QUEUE_TYPE::SAMPLE));
}

TEST_F(TestClass, Size) {
	EXPECT_EQ(QueueManager::Instance().Size(E_QUEUE_TYPE::SAMPLE), 0);

	QueueManager::Instance().Push(E_QUEUE_TYPE::SAMPLE, 1);

	EXPECT_EQ(QueueManager::Instance().Size(E_QUEUE_TYPE::SAMPLE), 1);
}

TEST_F(TestClass, total) {
	EXPECT_TRUE(QueueManager::Instance().Empty(E_QUEUE_TYPE::SAMPLE));

	const vector<int> vecData = {1, 2, 3};

	for (const auto& iter : vecData) {
		QueueManager::Instance().Push(E_QUEUE_TYPE::SAMPLE, iter);
	}

	EXPECT_EQ(QueueManager::Instance().Size(E_QUEUE_TYPE::SAMPLE), (int)vecData.size());

	EXPECT_FALSE(QueueManager::Instance().Empty(E_QUEUE_TYPE::SAMPLE));

	for (const auto& iter : vecData) {
		EXPECT_EQ(QueueManager::Instance().Front<int>(E_QUEUE_TYPE::SAMPLE), iter);

		QueueManager::Instance().Pop(E_QUEUE_TYPE::SAMPLE);
	}

	EXPECT_EQ(QueueManager::Instance().Size(E_QUEUE_TYPE::SAMPLE), 0);

	EXPECT_TRUE(QueueManager::Instance().Empty(E_QUEUE_TYPE::SAMPLE));
}
