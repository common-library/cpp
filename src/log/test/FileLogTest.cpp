#include "../FileLog.h"
#include "FileManager.h"
#include "test.h"
#include "gtest/gtest.h"
#include <cstring>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <ranges>
#include <string>
#include <vector>

using namespace std;

static void check(const LOG_LEVEL &logLevel) {
	FileLog fileLog;

	const vector<function<bool()>> loggings{[&fileLog]() { return fileLog.Debug("aaa"); },
											[&fileLog]() { return fileLog.Info("bbb"); },
											[&fileLog]() { return fileLog.Warning("ccc"); },
											[&fileLog]() { return fileLog.Error("ddd"); },
											[&fileLog]() { return fileLog.Critical("eee"); }};

	const map<bool, map<LOG_LEVEL, string>> answer = {
		{true,
		 {
			 {LOG_LEVEL::DEBUG, ", DEBUG, " + PROJECT_SOURCE_DIR +
									"/src/log/test/FileLogTest.cpp:19, check(const "
									"LOG_LEVEL&)::<lambda()>] : aaa\r"},
			 {LOG_LEVEL::INFO, ", INFO, " + PROJECT_SOURCE_DIR +
								   "/src/log/test/FileLogTest.cpp:20, check(const "
								   "LOG_LEVEL&)::<lambda()>] : bbb\r"},
			 {LOG_LEVEL::WARNING, ", WARNING, " + PROJECT_SOURCE_DIR +
									  "/src/log/test/FileLogTest.cpp:21, check(const "
									  "LOG_LEVEL&)::<lambda()>] : ccc\r"},
			 {LOG_LEVEL::ERROR, ", ERROR, " + PROJECT_SOURCE_DIR +
									"/src/log/test/FileLogTest.cpp:22, check(const "
									"LOG_LEVEL&)::<lambda()>] : ddd\r"},
			 {LOG_LEVEL::CRITICAL, ", CRITICAL, " + PROJECT_SOURCE_DIR +
									   "/src/log/test/FileLogTest.cpp:23, "
									   "check(const LOG_LEVEL&)::<lambda()>] : eee\r"},
		 }},
		{false,
		 {
			 {LOG_LEVEL::DEBUG, ", DEBUG] : aaa\r"},
			 {LOG_LEVEL::INFO, ", INFO] : bbb\r"},
			 {LOG_LEVEL::WARNING, ", WARNING] : ccc\r"},
			 {LOG_LEVEL::ERROR, ", ERROR] : ddd\r"},
			 {LOG_LEVEL::CRITICAL, ", CRITICAL] : eee\r"},
		 }}};

	const int count = 1000;
	const vector<string> outputPaths{FileManager::Instance().GetTempPath() + "/tmpXXXXXX_1",
									 FileManager::Instance().GetTempPath() + "/tmpXXXXXX_2"};
	const vector<string> fileNames{"test1", "test2"};

	for (const auto &iter : outputPaths) {
		EXPECT_TRUE(FileManager::Instance().MakeDir(iter));
	}

	EXPECT_TRUE(fileLog.Initialize(logLevel, outputPaths.at(0), fileNames.at(0), false, true));

	mutex m;
	const auto job = [&m, &fileLog, &count, &outputPaths, &fileNames](const auto &func) {
		for (int i = 0; i < count; ++i) {
			lock_guard<mutex> lock(m);

			fileLog.SetOutputPath(outputPaths.at(i % 2));
			fileLog.SetFileName(fileNames.at(i % 2));

			fileLog.SetLinePrint(true);
			fileLog.SetThreadMode(true);
			EXPECT_TRUE(func());

			fileLog.SetLinePrint(false);
			fileLog.SetThreadMode(false);
			EXPECT_TRUE(func());
		}
	};

	vector<future<void>> jobs = {};
	for (const auto &iter : loggings) {
		jobs.push_back(async(launch::async, bind(job, iter)));
	}

	for (auto &iter : jobs) {
		if (iter.valid()) {
			iter.get();
		}
	}
	jobs.clear();

	EXPECT_TRUE(fileLog.Flush());

	for (const auto &iter : outputPaths) {
		map<string, int> result;
		result.clear();
		for (const auto &iter2 : FileManager::Instance().GetSubDirectories(iter)) {
			const auto readResult = FileManager::Instance().Read(iter2);
			EXPECT_TRUE(get<0>(readResult));
			for (const auto &iter3 : ranges::views::split(get<1>(readResult), '\n')) {
				const auto temp = string(iter3.begin(), iter3.end());
				if (temp.empty()) {
					continue;
				}

				++result[temp.substr(strlen("[13:43:36"))];
			}
		}

		for (const auto &iter2 : answer) {
			for (const auto &iter3 : iter2.second) {
				if (static_cast<underlying_type_t<LOG_LEVEL>>(iter3.first) <
					static_cast<underlying_type_t<LOG_LEVEL>>(logLevel)) {
					continue;
				}

				if (result.find(iter3.second) == result.end()) {
					EXPECT_STREQ("invalid log", iter3.second.c_str());
				}
				EXPECT_EQ(result[iter3.second], count / 2);
			}
		}
	}

	for (const auto &iter : outputPaths) {
		EXPECT_TRUE(FileManager::Instance().RemoveAll(iter));
	}
}

TEST(FileLogTest, Initialize) {
	FileLog fileLog;

	EXPECT_TRUE(fileLog.Initialize(LOG_LEVEL::DEBUG, "/tmp", "test", true, true));

	EXPECT_EQ(LOG_LEVEL::DEBUG, fileLog.GetLogLevel());
	EXPECT_STREQ("/tmp", fileLog.GetOutputPath().c_str());
	EXPECT_STREQ("test", fileLog.GetFileName().c_str());
	EXPECT_TRUE(fileLog.GetLinePrint());
	EXPECT_TRUE(fileLog.GetThreadMode());
}

TEST(FileLogTest, Debug) { check(LOG_LEVEL::DEBUG); }

TEST(FileLogTest, Info) { check(LOG_LEVEL::INFO); }

TEST(FileLogTest, Warning) { check(LOG_LEVEL::WARNING); }

TEST(FileLogTest, Error) { check(LOG_LEVEL::ERROR); }

TEST(FileLogTest, Critical) { check(LOG_LEVEL::CRITICAL); }

TEST(FileLogTest, Flush) {
	FileLog fileLog;

	EXPECT_TRUE(fileLog.Flush());
}

TEST(FileLogTest, GetLogLevel) {
	FileLog fileLog;

	fileLog.SetLogLevel(LOG_LEVEL::DEBUG);

	EXPECT_EQ(LOG_LEVEL::DEBUG, fileLog.GetLogLevel());
}

TEST(FileLogTest, SetLogLevel) {
	FileLog fileLog;

	fileLog.SetLogLevel(LOG_LEVEL::DEBUG);

	EXPECT_EQ(LOG_LEVEL::DEBUG, fileLog.GetLogLevel());
}

TEST(FileLogTest, GetOutputPath) {
	FileLog fileLog;

	fileLog.SetOutputPath("/tmp");

	EXPECT_STREQ("/tmp", fileLog.GetOutputPath().c_str());
}

TEST(FileLogTest, SetOutputPath) {
	FileLog fileLog;

	fileLog.SetOutputPath("/tmp");

	EXPECT_STREQ("/tmp", fileLog.GetOutputPath().c_str());
}

TEST(FileLogTest, GetFileName) {
	FileLog fileLog;

	fileLog.SetFileName("test");

	EXPECT_STREQ("test", fileLog.GetFileName().c_str());
}

TEST(FileLogTest, SetFileName) {
	FileLog fileLog;

	fileLog.SetFileName("test");

	EXPECT_STREQ("test", fileLog.GetFileName().c_str());
}

TEST(FileLogTest, GetLinePrint) {
	FileLog fileLog;

	fileLog.SetLinePrint(true);

	EXPECT_TRUE(fileLog.GetLinePrint());
}

TEST(FileLogTest, SetLinePrint) {
	FileLog fileLog;

	fileLog.SetLinePrint(true);

	EXPECT_TRUE(fileLog.GetLinePrint());
}

TEST(FileLogTest, GetThreadMode) {
	FileLog fileLog;

	fileLog.SetThreadMode(true);

	EXPECT_TRUE(fileLog.GetThreadMode());
}

TEST(FileLogTest, SetThreadMode) {
	FileLog fileLog;

	fileLog.SetThreadMode(true);

	EXPECT_TRUE(fileLog.GetThreadMode());
}
