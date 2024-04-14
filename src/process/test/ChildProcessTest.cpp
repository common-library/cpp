#include "../ChildProcess.h"
#include "test.h"
#include "gtest/gtest.h"
#include <csignal>
#include <cstdlib>
#include <future>
#include <thread>

class ChildProcessForChild1 : public ChildProcess {
	private:
		virtual bool Initialize() override final { return false; }
		virtual bool Finalize() override final { return false; }

		virtual bool Job() override final { return true; }

	public:
		ChildProcessForChild1() = default;
		virtual ~ChildProcessForChild1() = default;
};

class ChildProcessForChild2 : public ChildProcess {
	private:
		virtual bool Initialize() override final { return true; }
		virtual bool Finalize() override final { return true; }

		virtual bool Job() override final {
			while (this->GetCondition()) {
				this_thread::sleep_for(1ms);
			}

			return true;
		}

	public:
		ChildProcessForChild2() = default;
		virtual ~ChildProcessForChild2() = default;
};

TEST(ChildProcessTest, total) {
	GTEST_FLAG_SET(death_test_style, "threadsafe");

	auto run = [](int howToStop) {
		ChildProcessForChild2 process;

		auto result = async(launch::async, [&process, &howToStop]() {
			while (process.GetCondition() == false) {
				this_thread::sleep_for(1ms);
			}

			switch (howToStop) {
			case 1:
				EXPECT_TRUE(process.Stop());
				break;
			case 2:
				EXPECT_EQ(kill(getpid(), SIGTERM), 0);
				break;
			}
		});

		ASSERT_TRUE(process.Start());

		result.wait();

		EXPECT_FALSE(process.GetCondition());
	};

	ChildProcessForChild1 process;

	EXPECT_FALSE(process.Start());
	EXPECT_FALSE(process.Stop());

	const pid_t pid = fork();
	ASSERT_NE(pid, -1);
	if (pid == 0) {
		run(1);
		run(2);

		exit(testing::Test::HasFailure());
	}

	EXPECT_TRUE(wait_process(pid));
}
