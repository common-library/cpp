#include "test.h"

#include "../ParentProcess.h"
#include "../EnvironmentVariable.h"

#include "gtest/gtest.h"

class EnvironmentVariableTest : public ::testing::Test {
protected:
	EnvironmentVariable environmentVariable;

	void SetUp() override {
		extern int optind;
		optind = 1;

		int iArgc = 4;
		char *pcArgv[] = {(char *)"./EnvironmentVariableTest", (char *)"-c", (char *)GstrConfigPath.c_str(), (char *)"-s"};

		EXPECT_TRUE(this->environmentVariable.Initialize(iArgc, pcArgv));
	}

	void TearDown() override {}
};

class EnvironmentVariableFalseTest : public ::testing::Test {
protected:
	EnvironmentVariable environmentVariable;

	void SetUp() override {
		extern int optind;
		optind = 1;

		int iArgc = 1;
		char *pcArgv[] = {(char *)"./EnvironmentVariableTest"};

		EXPECT_FALSE(this->environmentVariable.Initialize(iArgc, pcArgv));
	}

	void TearDown() override {}
};

TEST_F(EnvironmentVariableTest, Initialize)
{
	EXPECT_FALSE(this->environmentVariable.Initialize(0, nullptr));
}

TEST_F(EnvironmentVariableFalseTest, Initialize)
{
}

TEST_F(EnvironmentVariableTest, InitializeLog)
{
	EXPECT_TRUE(this->environmentVariable.InitializeLog());
}

TEST_F(EnvironmentVariableFalseTest, InitializeLog)
{
	EXPECT_FALSE(this->environmentVariable.InitializeLog());
}

TEST_F(EnvironmentVariableTest, Usage)
{
	EXPECT_STREQ(this->environmentVariable.Usage().c_str(), "usage: EnvironmentVariableTest -c config_path [-s]");
}

TEST_F(EnvironmentVariableTest, GetStandAlone)
{
	EXPECT_TRUE(this->environmentVariable.GetStandAlone());
}

TEST_F(EnvironmentVariableTest, GetConfigPath)
{
	EXPECT_STREQ(this->environmentVariable.GetConfigPath().c_str(), GstrConfigPath.c_str());
}

TEST_F(EnvironmentVariableTest, GetProcessName)
{
	EXPECT_STREQ(this->environmentVariable.GetProcessName().c_str(), "EnvironmentVariableTest");
}

TEST_F(EnvironmentVariableTest, GetGid)
{
	this->environmentVariable.SetGid(100);
	EXPECT_EQ(this->environmentVariable.GetGid(), 100);
}

TEST_F(EnvironmentVariableTest, SetGid)
{
	this->environmentVariable.SetGid(100);
	EXPECT_EQ(this->environmentVariable.GetGid(), 100);
}

TEST_F(EnvironmentVariableTest, GetUid)
{
	this->environmentVariable.SetUid(100);
	EXPECT_EQ(this->environmentVariable.GetUid(), 100);
}

TEST_F(EnvironmentVariableTest, SetUid)
{
	this->environmentVariable.SetUid(100);
	EXPECT_EQ(this->environmentVariable.GetUid(), 100);
}

TEST_F(EnvironmentVariableTest, GetParentPid)
{
	this->environmentVariable.SetParentPid(100);
	EXPECT_EQ(this->environmentVariable.GetParentPid(), 100);
}

TEST_F(EnvironmentVariableTest, SetParentPid)
{
	this->environmentVariable.SetParentPid(100);
	EXPECT_EQ(this->environmentVariable.GetParentPid(), 100);
}

TEST_F(EnvironmentVariableTest, GetChildPid)
{
	this->environmentVariable.SetChildPid(100);
	EXPECT_EQ(this->environmentVariable.GetChildPid(), 100);
}

TEST_F(EnvironmentVariableTest, SetChildPid)
{
	this->environmentVariable.SetChildPid(100);
	EXPECT_EQ(this->environmentVariable.GetChildPid(), 100);
}

TEST_F(EnvironmentVariableTest, GetCondition)
{
	this->environmentVariable.SetCondition(true);
	EXPECT_TRUE(this->environmentVariable.GetCondition());

	this->environmentVariable.SetCondition(false);
	EXPECT_FALSE(this->environmentVariable.GetCondition());
}

TEST_F(EnvironmentVariableTest, SetCondition)
{
	this->environmentVariable.SetCondition(true);
	EXPECT_TRUE(this->environmentVariable.GetCondition());

	this->environmentVariable.SetCondition(false);
	EXPECT_FALSE(this->environmentVariable.GetCondition());
}

TEST_F(EnvironmentVariableTest, GetProcess)
{
	EXPECT_TRUE(this->environmentVariable.GetProcess(E_PROCESS_TYPE::PARENT) == nullptr);

	ParentProcess parentProcess(nullptr);
	this->environmentVariable.SetProcess(E_PROCESS_TYPE::PARENT, &parentProcess);
	EXPECT_EQ(&parentProcess, this->environmentVariable.GetProcess(E_PROCESS_TYPE::PARENT));
}

TEST_F(EnvironmentVariableTest, SetProcess)
{
	ParentProcess parentProcess(nullptr);
	this->environmentVariable.SetProcess(E_PROCESS_TYPE::PARENT, &parentProcess);
	EXPECT_EQ(&parentProcess, this->environmentVariable.GetProcess(E_PROCESS_TYPE::PARENT));

	this->environmentVariable.SetProcess(E_PROCESS_TYPE::PARENT, nullptr);
	EXPECT_TRUE(this->environmentVariable.GetProcess(E_PROCESS_TYPE::PARENT) == nullptr);
}
