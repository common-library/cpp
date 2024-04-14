#include "../Signal.h"
#include "gtest/gtest.h"
#include <atomic>
#include <csignal>
#include <cstdlib>

using namespace std;

TEST(SignalTest, Add) {
	atomic_bool condition = true;

	Signal::Instance().Add(SIGINT, SIG_IGN);
	Signal::Instance().Add(SIGTERM, [&condition](int) { condition.store(false); });

	while (condition) {
		EXPECT_EQ(kill(getpid(), SIGTERM), 0);
	}
}

TEST(SignalTest, Delete) {
	Signal::Instance().Add(SIGINT, SIG_IGN);
	Signal::Instance().Add(SIGTERM, SIG_IGN);
	Signal::Instance().Delete();

	Signal::Instance().Add(SIGINT, SIG_IGN);
	Signal::Instance().Add(SIGTERM, SIG_IGN);
	Signal::Instance().Delete(SIGINT);
}

TEST(SignalTest, Handler) {
	Signal::Instance().Add(SIGCHLD, [](int) {});

	for (int i = 0; i < 10; ++i) {
		const pid_t pid = fork();
		ASSERT_NE(pid, -1);

		if (pid == 0) {
			Signal::Instance();
			EXPECT_EQ(signal(SIGCHLD, SIG_IGN), SIG_DFL);

			exit(testing::Test::HasFailure());
		} else {
			int status = 0;
			ASSERT_NE(waitpid(pid, &status, 0), -1);
			EXPECT_NE(WIFEXITED(status), 0);
			EXPECT_EQ(WEXITSTATUS(status), 0);
		}
	}
}

TEST(SignalTest, Instance) {
	for (int i = 0; i < 100; ++i) {
		EXPECT_EQ(&Signal::Instance(), &Signal::Instance());
	}
}
