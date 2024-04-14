#pragma once

#include <atomic>
#include <condition_variable>
#include <future>
#include <map>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

enum class LOG_LEVEL {
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR,
	CRITICAL,
};

static const map<LOG_LEVEL, string_view> LOG_LEVEL_INFO = {
	{LOG_LEVEL::DEBUG, "DEBUG"}, {LOG_LEVEL::INFO, "INFO"},			{LOG_LEVEL::WARNING, "WARNING"},
	{LOG_LEVEL::ERROR, "ERROR"}, {LOG_LEVEL::CRITICAL, "CRITICAL"},
};

class FileLog {
	private:
		atomic_bool condition;
		atomic_bool initialize;

		LOG_LEVEL logLevel;
		string outputPath;
		string fileName;
		atomic_bool linePrint;
		atomic_bool threadMode;

		mutable mutex mutexForSettings;
		mutex mutexForCv;

		mutex mutexForJobs;
		vector<future<error_code>> jobs;

		future<void> futureForFlush;

		condition_variable cv;

		string MakeLog(const LOG_LEVEL &logLevel, const bool &linePrint, const string &log,
					   const tm &sTm, const source_location &sourceLocation);

		string MakeFullPath(const string &outputPath, const string &fileName, const tm &sTm);

		bool Print(const LOG_LEVEL &logLevel, const string &outputPath, const string &fileName,
				   const bool &linePrint, const string &log, const time_t &time,
				   const source_location &sourceLocation);

		bool Logging(const LOG_LEVEL &logLevel, const string &log,
					 const source_location &sourceLocation);

	public:
		FileLog();

		~FileLog();

		bool Initialize(const LOG_LEVEL &logLevel, const string &outputPath, const string &fileName,
						const bool &linePrint, const bool &threadMode);

		bool Debug(const string &log,
				   const source_location &sourceLocation = source_location::current());
		bool Info(const string &log,
				  const source_location &sourceLocation = source_location::current());
		bool Warning(const string &log,
					 const source_location &sourceLocation = source_location::current());
		bool Error(const string &log,
				   const source_location &sourceLocation = source_location::current());
		bool Critical(const string &log,
					  const source_location &sourceLocation = source_location::current());

		bool Flush();

		LOG_LEVEL GetLogLevel() const;
		void SetLogLevel(const LOG_LEVEL &logLevel);

		string GetOutputPath() const;
		void SetOutputPath(const string &outputPath);

		string GetFileName() const;
		void SetFileName(const string &fileName);

		bool GetLinePrint() const;
		void SetLinePrint(const bool &linePrint);

		bool GetThreadMode() const;
		void SetThreadMode(const bool &threadMode);
};
