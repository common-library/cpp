#include "../ParentProcess.h"
#include "../ChildProcess.h"
#include "../EnvironmentVariable.h"
#include "../Signal.h"
#include "test.h"
#include "gtest/gtest.h"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

class ChildProcess1 : public ChildProcess {
	private:
		int job;

		virtual bool Initialize() override final { return true; }
		virtual bool Finalize() override final { return true; }

		virtual bool Job() override {
			while (this->GetCondition()) {
				switch (this->job) {
				case 1:
					this_thread::sleep_for(1ms);
					break;
				case 2:
					return true;
				case 3:
					printf("%s", 1);
					break;
				}
			}

			return true;
		}

	public:
		ChildProcess1(const int &job) : job(job) {}
		virtual ~ChildProcess1() = default;
};

bool test(int argc, char *argv[], int howToStop, int job) {
	auto run = [argc, argv, howToStop, job]() {
		const pid_t pid = fork();
		EXPECT_NE(pid, -1);

		if (pid == 0) {
			vector<unique_ptr<ChildProcess>> processes = {};
			for (int i = 0; i < 1; ++i) {
				processes.push_back(make_unique<ChildProcess1>(job));
			}

			EnvironmentVariable environmentVariable;
			EXPECT_TRUE(environmentVariable.Initialize(argc, argv));

			ParentProcess parentProcess(environmentVariable.GetStandAlone(),
										environmentVariable.GetBinaryName(),
										processes);

			auto result = async(launch::async, [&parentProcess, &howToStop]() {
				while (parentProcess.GetCondition() == false) {
					this_thread::sleep_for(1ms);
				}

				switch (howToStop) {
				case 1:
					EXPECT_TRUE(parentProcess.Stop());
					break;
				case 2:
					EXPECT_EQ(kill(getpid(), SIGTERM), 0);
					break;
				}
			});

			EXPECT_TRUE(parentProcess.Start());

			EXPECT_FALSE(parentProcess.GetCondition());

			exit(testing::Test::HasFailure());
		} else {
			EXPECT_TRUE(wait_process(pid));
		}
	};

	const pid_t pid = fork();
	EXPECT_NE(pid, -1);
	if (pid == 0) {
		run();

		exit(testing::Test::HasFailure());
	}

	return wait_process(pid);
}

TEST(ParentProcessTest, NonStandalone) {
	GTEST_FLAG_SET(death_test_style, "threadsafe");

	int argc = 3;
	char *argv[] = {(char *)"non_standalone", (char *)"-c",
					(char *)CONFIG_PATH.c_str()};

	EXPECT_TRUE(test(argc, argv, 1, 1));
	EXPECT_TRUE(test(argc, argv, 2, 1));
}

TEST(ParentProcessTest, Standalone) {
	GTEST_FLAG_SET(death_test_style, "threadsafe");

	int argc = 4;
	char *argv[] = {(char *)"standalone", (char *)"-c",
					(char *)CONFIG_PATH.c_str(), (char *)"-s"};

	EXPECT_TRUE(test(argc, argv, 1, 2));
	EXPECT_TRUE(test(argc, argv, 2, 2));
}

TEST(ParentProcessTest, sigchld) {
	GTEST_FLAG_SET(death_test_style, "threadsafe");

	int argc = 3;
	char *argv[] = {(char *)"non_standalone", (char *)"-c",
					(char *)CONFIG_PATH.c_str()};

	EXPECT_TRUE(test(argc, argv, 2, 3));
}

TEST(ParentProcessTest, twice_start) {}
