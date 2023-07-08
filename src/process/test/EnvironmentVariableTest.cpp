#include "../EnvironmentVariable.h"
#include "../ParentProcess.h"
#include "test.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

using namespace std;

class EnvironmentVariableTest : public ::testing::Test {
	protected:
		const string binaryName = "binary_name";
		const string binaryPath = "./aaa/bbb/ccc/" + binaryName;

		EnvironmentVariable environmentVariable;

		void SetUp() override {
			EXPECT_TRUE(this->environmentVariable.Initialize(0, nullptr));

			int argc1 = 3;
			char *argv1[] = {(char *)binaryPath.c_str(), (char *)"-c",
							 (char *)""};
			EXPECT_FALSE(this->environmentVariable.Initialize(argc1, argv1));

			int argc2 = 3;
			char *argv2[] = {(char *)binaryPath.c_str(), (char *)"-c",
							 (char *)"invalid"};
			EXPECT_FALSE(this->environmentVariable.Initialize(argc2, argv2));
		}

		void TearDown() override {
			EXPECT_STREQ(this->environmentVariable.Usage().c_str(),
						 ("usage: " + this->binaryName + " -c config_path [-s]")
							 .c_str());
			EXPECT_STREQ(this->environmentVariable.GetConfigPath().c_str(),
						 CONFIG_PATH.c_str());
			EXPECT_STREQ(this->environmentVariable.GetBinaryName().c_str(),
						 binaryName.c_str());
		}
};

TEST_F(EnvironmentVariableTest, Initialize_1) {
	int argc = 3;
	char *argv[] = {(char *)binaryPath.c_str(), (char *)"-c",
					(char *)CONFIG_PATH.c_str()};

	EXPECT_TRUE(this->environmentVariable.Initialize(argc, argv));

	EXPECT_FALSE(environmentVariable.GetStandAlone());
}

TEST_F(EnvironmentVariableTest, Initialize_2) {
	vector<string> args = {};

	args = {binaryPath, "-c"};
	EXPECT_FALSE(this->environmentVariable.Initialize(args));

	args = {binaryPath, "-c", CONFIG_PATH, "-a"};
	EXPECT_FALSE(this->environmentVariable.Initialize(args));

	args = {binaryPath, "-c", CONFIG_PATH, "-s"};
	EXPECT_TRUE(this->environmentVariable.Initialize(args));
	EXPECT_TRUE(this->environmentVariable.GetStandAlone());
}
