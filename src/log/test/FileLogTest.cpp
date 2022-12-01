#include "test.h"

#include "../FileLog.h"

#include "gtest/gtest.h"

#include "FileManager.h"

static bool check(const E_LOG_LEVEL& eLogLevel, const bool& bThread) {
	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char*)(strTemplate.c_str()));

	const string strLocal = "local";
	const string strGlobal = "global";

	FileLog fileLog;

	EXPECT_TRUE(fileLog.Initialize(eLogLevel, strOutputPath, strLocal, bThread));
	EXPECT_TRUE(Singleton<FileLog>::Instance().Initialize(eLogLevel, strOutputPath,
														  strGlobal, bThread));

	EXPECT_TRUE(DEBUG(fileLog, "(%d) (%s)", 1, "a"));
	EXPECT_TRUE(DEBUG_L(fileLog, "(%d) (%s)", 11, "aa"));
	EXPECT_TRUE(DEBUG_G("(%d) (%s)", 1, "a"));
	EXPECT_TRUE(DEBUG_L_G("(%d) (%s)", 11, "aa"));

	EXPECT_TRUE(INFO(fileLog, "(%d) (%s)", 2, "b"));
	EXPECT_TRUE(INFO_L(fileLog, "(%d) (%s)", 22, "bb"));
	EXPECT_TRUE(INFO_G("(%d) (%s)", 2, "b"));
	EXPECT_TRUE(INFO_L_G("(%d) (%s)", 22, "bb"));

	EXPECT_TRUE(WARNING(fileLog, "(%d) (%s)", 3, "c"));
	EXPECT_TRUE(WARNING_L(fileLog, "(%d) (%s)", 33, "cc"));
	EXPECT_TRUE(WARNING_G("(%d) (%s)", 3, "c"));
	EXPECT_TRUE(WARNING_L_G("(%d) (%s)", 33, "cc"));

	EXPECT_TRUE(ERROR(fileLog, "(%d) (%s)", 4, "d"));
	EXPECT_TRUE(ERROR_L(fileLog, "(%d) (%s)", 44, "dd"));
	EXPECT_TRUE(ERROR_G("(%d) (%s)", 4, "d"));
	EXPECT_TRUE(ERROR_L_G("(%d) (%s)", 44, "dd"));

	EXPECT_TRUE(CRITICAL(fileLog, "(%d) (%s)", 5, "e"));
	EXPECT_TRUE(CRITICAL_L(fileLog, "(%d) (%s)", 55, "ee"));
	EXPECT_TRUE(CRITICAL_G("(%d) (%s)", 5, "e"));
	EXPECT_TRUE(CRITICAL_L_G("(%d) (%s)", 55, "ee"));

	EXPECT_TRUE(fileLog.Flush());
	EXPECT_TRUE(Singleton<FileLog>::Instance().Flush());

	string strLocalResult = "";
	string strGlobalResult = "";
	for (const auto& iter : FileManager().GetPathList(strOutputPath)) {
		if (iter.find(strLocal) != string::npos) {
			EXPECT_TRUE(FileManager().Read(iter, strLocalResult));
		} else if (iter.find(strGlobal) != string::npos) {
			EXPECT_TRUE(FileManager().Read(iter, strGlobalResult));
		}
	}

	map<string, multimap<E_LOG_LEVEL, string>> mapResult = {
		{strLocal,
		 {{E_LOG_LEVEL::DEBUG, ", DEBUG] :  (1) (a)\r\n"},
		  {E_LOG_LEVEL::DEBUG, ", DEBUG] :  (11) (aa) (" + GstrSourceDir +
								   "/src/log/test/FileLogTest.cpp:23)\r\n"},

		  {E_LOG_LEVEL::INFO, ", INFO] :  (2) (b)\r\n"},
		  {E_LOG_LEVEL::INFO, ", INFO] :  (22) (bb) (" + GstrSourceDir +
								  "/src/log/test/FileLogTest.cpp:28)\r\n"},

		  {E_LOG_LEVEL::WARNING, ", WARNING] :  (3) (c)\r\n"},
		  {E_LOG_LEVEL::WARNING, ", WARNING] :  (33) (cc) (" + GstrSourceDir +
									 "/src/log/test/FileLogTest.cpp:33)\r\n"},

		  {E_LOG_LEVEL::ERROR, ", ERROR] :  (4) (d)\r\n"},
		  {E_LOG_LEVEL::ERROR, ", ERROR] :  (44) (dd) (" + GstrSourceDir +
								   "/src/log/test/FileLogTest.cpp:38)\r\n"},

		  {E_LOG_LEVEL::CRITICAL, ", CRITICAL] :  (5) (e)\r\n"},
		  {E_LOG_LEVEL::CRITICAL, ", CRITICAL] :  (55) (ee) (" + GstrSourceDir +
									  "/src/log/test/FileLogTest.cpp:43)\r\n"}}},
		{strGlobal,
		 {{E_LOG_LEVEL::DEBUG, ", DEBUG] :  (1) (a)\r\n"},
		  {E_LOG_LEVEL::DEBUG, ", DEBUG] :  (11) (aa) (" + GstrSourceDir +
								   "/src/log/test/FileLogTest.cpp:25)\r\n"},

		  {E_LOG_LEVEL::INFO, ", INFO] :  (2) (b)\r\n"},
		  {E_LOG_LEVEL::INFO, ", INFO] :  (22) (bb) (" + GstrSourceDir +
								  "/src/log/test/FileLogTest.cpp:30)\r\n"},

		  {E_LOG_LEVEL::WARNING, ", WARNING] :  (3) (c)\r\n"},
		  {E_LOG_LEVEL::WARNING, ", WARNING] :  (33) (cc) (" + GstrSourceDir +
									 "/src/log/test/FileLogTest.cpp:35)\r\n"},

		  {E_LOG_LEVEL::ERROR, ", ERROR] :  (4) (d)\r\n"},
		  {E_LOG_LEVEL::ERROR, ", ERROR] :  (44) (dd) (" + GstrSourceDir +
								   "/src/log/test/FileLogTest.cpp:40)\r\n"},

		  {E_LOG_LEVEL::CRITICAL, ", CRITICAL] :  (5) (e)\r\n"},
		  {E_LOG_LEVEL::CRITICAL, ", CRITICAL] :  (55) (ee) (" + GstrSourceDir +
									  "/src/log/test/FileLogTest.cpp:45)\r\n"}}}};

	for (const auto& iter : mapResult) {
		for (const auto& iter2 : iter.second) {
			if (static_cast<underlying_type_t<E_LOG_LEVEL>>(iter2.first) <
				static_cast<underlying_type_t<E_LOG_LEVEL>>(eLogLevel)) {
				continue;
			}

			string strResult = "";
			if (iter.first == strLocal) {
				strResult = strLocalResult;
			} else {
				strResult = strGlobalResult;
			}

			if (strResult.find(iter2.second) == string::npos) {
				EXPECT_STREQ(iter2.second.c_str(), strResult.c_str());
			}
		}
	}

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));

	return true;
}

TEST(FileLogTest, Initialize) {
	FileLog fileLog;

	EXPECT_TRUE(fileLog.Initialize(E_LOG_LEVEL::INFO, "", "", true));
	EXPECT_TRUE(fileLog.Initialize(E_LOG_LEVEL::INFO, "", "", false));

	const string strTemplate = FileManager().GetTempPath() + "/tmpXXXXXX";
	const string strOutputPath = mkdtemp((char*)(strTemplate.c_str()));

	EXPECT_TRUE(fileLog.Initialize(E_LOG_LEVEL::INFO, strOutputPath, "", true));
	EXPECT_TRUE(fileLog.Initialize(E_LOG_LEVEL::INFO, strOutputPath, "", false));

	EXPECT_TRUE(FileManager().RemoveAll(strOutputPath));
}

TEST(FileLogTest, Flush) {
	FileLog fileLog;

	EXPECT_TRUE(fileLog.Flush());
}

TEST(FileLogTest, SetThread) {
	Singleton<FileLog>::Instance().SetThread(true);
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, true));

	Singleton<FileLog>::Instance().SetThread(true);
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, false));

	Singleton<FileLog>::Instance().SetThread(false);
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, true));

	Singleton<FileLog>::Instance().SetThread(false);
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, false));
}

TEST(FileLogTest, DEBUG) {
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, true));
	EXPECT_TRUE(check(E_LOG_LEVEL::DEBUG, false));
}

TEST(FileLogTest, INFO) {
	EXPECT_TRUE(check(E_LOG_LEVEL::INFO, true));
	EXPECT_TRUE(check(E_LOG_LEVEL::INFO, false));
}

TEST(FileLogTest, WARNING) {
	EXPECT_TRUE(check(E_LOG_LEVEL::WARNING, true));
	EXPECT_TRUE(check(E_LOG_LEVEL::WARNING, false));
}

TEST(FileLogTest, ERROR) {
	EXPECT_TRUE(check(E_LOG_LEVEL::ERROR, true));
	EXPECT_TRUE(check(E_LOG_LEVEL::ERROR, false));
}

TEST(FileLogTest, CRITICAL) {
	EXPECT_TRUE(check(E_LOG_LEVEL::CRITICAL, true));
	EXPECT_TRUE(check(E_LOG_LEVEL::CRITICAL, false));
}
