#include "test.h"

#include "../ParentProcess.h"

#include "gtest/gtest.h"

#include "Singleton.h"
#include "ThreadPool.h"
#include "../EnvironmentVariable.h"

class StandaloneTestChildProcess : public ChildProcess {
private:
	virtual bool InitializeDerived() {return true;}
	virtual bool FinalizeDerived() {return true;}
	virtual bool Job() {return raise(SIGTERM) == 0 ? true : false;}
public:
	StandaloneTestChildProcess() = default;
	virtual ~StandaloneTestChildProcess() = default;
};

class NonStandaloneTestChildProcess : public ChildProcess {
private:
	virtual bool InitializeDerived() {return true;}
	virtual bool FinalizeDerived() {return true;}
	virtual bool Job() {return true;}
public:
	NonStandaloneTestChildProcess() = default;
	virtual ~NonStandaloneTestChildProcess() = default;
};

class StandaloneParentProcessTest : public ::testing::Test {
protected:
	void SetUp() override {
		extern int optind;
		optind = 1;

		int iArgc = 4;
		char *pcArgv[] = {(char *)"./StandaloneParentProcessTest", (char *)"-c", (char *)GstrConfigPath.c_str(), (char *)"-s"};

		EXPECT_TRUE(Singleton<EnvironmentVariable>::Instance().Initialize(iArgc, pcArgv));
	}

	void TearDown() override {}
};

class NonStandaloneParentProcessTest : public ::testing::Test {
protected:
	void SetUp() override {
		extern int optind;
		optind = 1;

		int iArgc = 3;
		char *pcArgv[] = {(char *)"./NonStandaloneParentProcessTest", (char *)"-c", (char *)GstrConfigPath.c_str()};

		EXPECT_TRUE(Singleton<EnvironmentVariable>::Instance().Initialize(iArgc, pcArgv));
	}

	void TearDown() override {}
};

TEST_F(StandaloneParentProcessTest, SigTerm)
{
	const int iPid = fork();
	ASSERT_NE(iPid, -1);

	if(iPid == 0) {
		EXPECT_TRUE(ParentProcess(make_unique<StandaloneTestChildProcess>()).Start());

		exit(testing::Test::HasFailure());
	}
	this_thread::sleep_for(chrono::seconds(2));
}

TEST_F(NonStandaloneParentProcessTest, SigTerm)
{
	const int iPid = fork();
	ASSERT_NE(iPid, -1);

	if(iPid == 0) {
		EXPECT_TRUE(ParentProcess(make_unique<NonStandaloneTestChildProcess>()).Start());

		exit(testing::Test::HasFailure());
	}
	this_thread::sleep_for(chrono::seconds(2));

	EXPECT_EQ(kill(iPid, SIGTERM), 0);

	this_thread::sleep_for(chrono::seconds(1));
}

TEST_F(NonStandaloneParentProcessTest, twice_start)
{
	const int iPid = fork();
	ASSERT_NE(iPid, -1);

	if(iPid == 0) {
		EXPECT_TRUE(ParentProcess(make_unique<NonStandaloneTestChildProcess>()).Start());

		exit(testing::Test::HasFailure());
	}
	this_thread::sleep_for(chrono::seconds(2));

	EXPECT_FALSE(ParentProcess(nullptr).Start());

	EXPECT_EQ(kill(iPid, SIGTERM), 0);

	this_thread::sleep_for(chrono::seconds(1));
}

TEST_F(NonStandaloneParentProcessTest, Start)
{
	EXPECT_TRUE(ParentProcess(nullptr).Start());

	ThreadPool threadPool(1);

	ParentProcess parentProcess(make_unique<NonStandaloneTestChildProcess>());

	future<bool> future = threadPool.AddJob([&]()->bool{return parentProcess.Start();});
	this_thread::sleep_for(chrono::seconds(2));

	EXPECT_TRUE(parentProcess.Stop());

	EXPECT_TRUE(future.get());
}

TEST_F(NonStandaloneParentProcessTest, Stop)
{
	EXPECT_TRUE(ParentProcess(nullptr).Stop());
	EXPECT_TRUE(ParentProcess(make_unique<NonStandaloneTestChildProcess>()).Stop());
}
