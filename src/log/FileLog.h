#pragma once

#include <map>
using namespace std;

#include <Singleton.h>

#include <ThreadPool.h>

enum class E_LOG_LEVEL {
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR,
	CRITICAL,
};

static const map<E_LOG_LEVEL, string> GmapLogLevelInfo = {
	{E_LOG_LEVEL::DEBUG, "DEBUG"},		 {E_LOG_LEVEL::INFO, "INFO"},
	{E_LOG_LEVEL::WARNING, "WARNING"},	 {E_LOG_LEVEL::ERROR, "ERROR"},
	{E_LOG_LEVEL::CRITICAL, "CRITICAL"},
};

class FileLog {
	private:
		mutex mutexLock;

		E_LOG_LEVEL eLogLevel;

		string strFileName;
		string strOutputPath;
		string strFileNamePrefix;

		atomic<bool> bThread;
		vector<future<bool>> vecFuture;
		unique_ptr<ThreadPool> uniqptrThreadPool;

		bool Finalize();

		bool IsPrint(const E_LOG_LEVEL& eLogLevel);

		string MakeFileName(const time_t& sTime);
		string MakePrefixLog(const time_t& sTime, const E_LOG_LEVEL& eLogLevel);

	public:
		FileLog();
		virtual ~FileLog();

		bool Initialize(const E_LOG_LEVEL& eLogLevel, const string& strOutputPath,
						const string& strFileNamePrefix, const bool& bThread);

		bool Logging(const E_LOG_LEVEL& eLogLevel, const string& strFormat, ...);
		bool LoggingWithSourceLocation(const E_LOG_LEVEL& eLogLevel,
									   const string& strFileName, const int& iFileLine,
									   const string& strFormat, ...);

		bool Flush();

		void SetThread(const bool& bThread);
};

#define DEBUG(fileLog, args...) fileLog.Logging(E_LOG_LEVEL::DEBUG, ##args)
#define DEBUG_L(fileLog, args...)                                                        \
	fileLog.LoggingWithSourceLocation(E_LOG_LEVEL::DEBUG, __builtin_FILE(),              \
									  __builtin_LINE(), ##args)
#define DEBUG_G(args...)                                                                 \
	Singleton<FileLog>::Instance().Logging(E_LOG_LEVEL::DEBUG, ##args)
#define DEBUG_L_G(args...)                                                               \
	Singleton<FileLog>::Instance().LoggingWithSourceLocation(                            \
		E_LOG_LEVEL::DEBUG, __builtin_FILE(), __builtin_LINE(), ##args)

#define INFO(fileLog, args...) fileLog.Logging(E_LOG_LEVEL::INFO, ##args)
#define INFO_L(fileLog, args...)                                                         \
	fileLog.LoggingWithSourceLocation(E_LOG_LEVEL::INFO, __builtin_FILE(),               \
									  __builtin_LINE(), ##args)
#define INFO_G(args...) Singleton<FileLog>::Instance().Logging(E_LOG_LEVEL::INFO, ##args)
#define INFO_L_G(args...)                                                                \
	Singleton<FileLog>::Instance().LoggingWithSourceLocation(                            \
		E_LOG_LEVEL::INFO, __builtin_FILE(), __builtin_LINE(), ##args)

#define WARNING(fileLog, args...) fileLog.Logging(E_LOG_LEVEL::WARNING, ##args)
#define WARNING_L(fileLog, args...)                                                      \
	fileLog.LoggingWithSourceLocation(E_LOG_LEVEL::WARNING, __builtin_FILE(),            \
									  __builtin_LINE(), ##args)
#define WARNING_G(args...)                                                               \
	Singleton<FileLog>::Instance().Logging(E_LOG_LEVEL::WARNING, ##args)
#define WARNING_L_G(args...)                                                             \
	Singleton<FileLog>::Instance().LoggingWithSourceLocation(                            \
		E_LOG_LEVEL::WARNING, __builtin_FILE(), __builtin_LINE(), ##args)

#define ERROR(fileLog, args...) fileLog.Logging(E_LOG_LEVEL::ERROR, ##args)
#define ERROR_L(fileLog, args...)                                                        \
	fileLog.LoggingWithSourceLocation(E_LOG_LEVEL::ERROR, __builtin_FILE(),              \
									  __builtin_LINE(), ##args)
#define ERROR_G(args...)                                                                 \
	Singleton<FileLog>::Instance().Logging(E_LOG_LEVEL::ERROR, ##args)
#define ERROR_L_G(args...)                                                               \
	Singleton<FileLog>::Instance().LoggingWithSourceLocation(                            \
		E_LOG_LEVEL::ERROR, __builtin_FILE(), __builtin_LINE(), ##args)

#define CRITICAL(fileLog, args...) fileLog.Logging(E_LOG_LEVEL::CRITICAL, ##args)
#define CRITICAL_L(fileLog, args...)                                                     \
	fileLog.LoggingWithSourceLocation(E_LOG_LEVEL::CRITICAL, __builtin_FILE(),           \
									  __builtin_LINE(), ##args)
#define CRITICAL_G(args...)                                                              \
	Singleton<FileLog>::Instance().Logging(E_LOG_LEVEL::CRITICAL, ##args)
#define CRITICAL_L_G(args...)                                                            \
	Singleton<FileLog>::Instance().LoggingWithSourceLocation(                            \
		E_LOG_LEVEL::CRITICAL, __builtin_FILE(), __builtin_LINE(), ##args)
