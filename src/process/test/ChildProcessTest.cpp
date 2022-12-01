#include "test.h"

#include "../ChildProcess.h"

#include "gtest/gtest.h"

#include "../EnvironmentVariable.h"
#include "Singleton.h"
#include "ThreadPool.h"

class TestChildProcess : public ChildProcess {
	private:
		virtual bool InitializeDerived() { return true; }
		virtual bool FinalizeDerived() { return true; }
		virtual bool Job() { return true; }

	public:
		TestChildProcess() = default;
		virtual ~TestChildProcess() = default;
};

class FalseTestChildProcess : public ChildProcess {
	private:
		virtual bool InitializeDerived() { return false; }
		virtual bool FinalizeDerived() { return false; }
		virtual bool Job() { return false; }

	public:
		FalseTestChildProcess() = default;
		virtual ~FalseTestChildProcess() = default;
};

class FalseChildProcessTest : public ::testing::Test {
	protected:
		void SetUp() override {
			extern int optind;
			optind = 1;

			int iArgc = 1;
			char* pcArgv[] = {(char*)"./FalseChildProcessTest"};

			EXPECT_FALSE(
				Singleton<EnvironmentVariable>::Instance().Initialize(iArgc, pcArgv));
		}

		void TearDown() override {}
};

class StandaloneChildProcessTest : public ::testing::Test {
	protected:
		void SetUp() override {
			extern int optind;
			optind = 1;

			int iArgc = 4;
			char* pcArgv[] = {(char*)"./StandaloneChildProcessTest", (char*)"-c",
							  (char*)GstrConfigPath.c_str(), (char*)"-s"};

			EXPECT_TRUE(
				Singleton<EnvironmentVariable>::Instance().Initialize(iArgc, pcArgv));
		}

		void TearDown() override {}
};

class NonStandaloneChildProcessTest : public ::testing::Test {
	protected:
		void SetUp() override {
			extern int optind;
			optind = 1;

			int iArgc = 3;
			char* pcArgv[] = {(char*)"./NonStandaloneChildProcessTest", (char*)"-c",
							  (char*)GstrConfigPath.c_str()};

			EXPECT_TRUE(
				Singleton<EnvironmentVariable>::Instance().Initialize(iArgc, pcArgv));
		}

		void TearDown() override {}
};

TEST_F(StandaloneChildProcessTest, SigTerm) {
	const int iPid = fork();
	ASSERT_NE(iPid, -1);

	if (iPid == 0) {
		EXPECT_TRUE(TestChildProcess().Start());

		exit(testing::Test::HasFailure());
	}
	this_thread::sleep_for(chrono::seconds(1));

	EXPECT_EQ(kill(iPid, SIGTERM), 0);

	this_thread::sleep_for(chrono::seconds(1));
}

TEST_F(NonStandaloneChildProcessTest, Start) {
	FalseTestChildProcess falseTestChildProcess;
	EXPECT_FALSE(falseTestChildProcess.Start());

	ThreadPool threadPool(1);

	TestChildProcess testChildProcess;

	future<bool> future =
		threadPool.AddJob([&]() -> bool { return testChildProcess.Start(); });
	this_thread::sleep_for(chrono::seconds(1));

	EXPECT_TRUE(testChildProcess.Stop());

	EXPECT_TRUE(future.get());
}

TEST_F(NonStandaloneChildProcessTest, Stop) {
	TestChildProcess testChildProcess;
	EXPECT_TRUE(testChildProcess.Stop());
}
