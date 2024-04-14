#include "FileLog.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <format>
#include <future>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string>

using namespace std;

FileLog::FileLog() : condition(true), initialize(false) {
	this->futureForFlush = async(launch::async, [this]() {
		while (this->condition) {
			unique_lock<mutex> lock(this->mutexForCv);

			this->cv.wait(lock, [&]() { return this->jobs.size() || this->condition == false; });

			this->Flush();
		}
	});
}

FileLog::~FileLog() {
	this->condition.store(false);
	this->initialize.store(false);

	this->cv.notify_all();

	this->futureForFlush.get();
}

bool FileLog::Initialize(const LOG_LEVEL &logLevel, const string &outputPath,
						 const string &fileName, const bool &linePrint, const bool &threadMode) {
	lock_guard<mutex> lock(this->mutexForSettings);

	this->logLevel = logLevel;
	this->outputPath = outputPath;
	this->fileName = fileName;
	this->linePrint = linePrint;
	this->threadMode.store(threadMode);

	this->initialize.store(true);

	return true;
}

string FileLog::MakeLog(const LOG_LEVEL &logLevel, const bool &linePrint, const string &log,
						const tm &sTm, const source_location &sourceLocation) {
	stringstream ss;

	ss << put_time(&sTm, "%H:%M:%S");

	if (linePrint) {
		return format("[{}, {}, {}:{}, {}] : {}\r\n", ss.str(), LOG_LEVEL_INFO.at(logLevel),
					  sourceLocation.file_name(), to_string(sourceLocation.line()),
					  sourceLocation.function_name(), log);
	}

	return format("[{}, {}] : {}\r\n", ss.str(), LOG_LEVEL_INFO.at(logLevel), log);
}

string FileLog::MakeFullPath(const string &outputPath, const string &fileName, const tm &sTm) {
	stringstream ss;

	ss << put_time(&sTm, "%Y%m%d.log");

	string finalFileName = this->outputPath.size() ? this->outputPath + "/" : "  ";
	finalFileName += this->fileName.size() ? this->fileName + string("_") : "";
	finalFileName += ss.str();

	return finalFileName;
}

bool FileLog::Print(const LOG_LEVEL &logLevel, const string &outputPath, const string &fileName,
					const bool &linePrint, const string &log, const time_t &time,
					const source_location &sourceLocation) {
	tm sTm;
	if (localtime_r(&time, &sTm) == nullptr) {
		return false;
	}

	const string finalLog = this->MakeLog(logLevel, linePrint, log, sTm, sourceLocation);
	if (this->initialize == false) {
		printf("%s", finalLog.c_str());
		return true;
	}

	{
		lock_guard<mutex> lock(this->mutexForJobs);

		const string finalFullPath = this->MakeFullPath(outputPath, fileName, sTm);
		jobs.push_back(async(launch::deferred, [finalFullPath, finalLog]() {
			return FileManager::Instance().Write(finalFullPath, finalLog, ios::app);
		}));
	}

	if (this->threadMode) {
		this->cv.notify_one();
	} else {
		return this->Flush();
	}

	return true;
}

bool FileLog::Logging(const LOG_LEVEL &logLevel, const string &log,
					  const source_location &sourceLocation) {
	lock_guard<mutex> lock(this->mutexForSettings);

	if (static_cast<underlying_type_t<LOG_LEVEL>>(logLevel) <
		static_cast<underlying_type_t<LOG_LEVEL>>(this->logLevel)) {
		return true;
	}

	return this->Print(logLevel, this->outputPath, this->fileName, this->linePrint, log,
					   time(nullptr), sourceLocation);
}

bool FileLog::Debug(const string &log, const source_location &sourceLocation) {
	return this->Logging(LOG_LEVEL::DEBUG, log, sourceLocation);
}

bool FileLog::Info(const string &log, const source_location &sourceLocation) {
	return this->Logging(LOG_LEVEL::INFO, log, sourceLocation);
}

bool FileLog::Warning(const string &log, const source_location &sourceLocation) {
	return this->Logging(LOG_LEVEL::WARNING, log, sourceLocation);
}

bool FileLog::Error(const string &log, const source_location &sourceLocation) {
	return this->Logging(LOG_LEVEL::ERROR, log, sourceLocation);
}

bool FileLog::Critical(const string &log, const source_location &sourceLocation) {
	return this->Logging(LOG_LEVEL::CRITICAL, log, sourceLocation);
}

bool FileLog::Flush() {
	lock_guard<mutex> lock(this->mutexForJobs);

	for (auto &iter : this->jobs) {
		if (iter.valid()) {
			if (iter.get() == false) {
				printf("log error - errno : (%d), strerror : (%s)\n", errno, strerror(errno));
			}
		}
	}
	this->jobs.clear();

	return true;
}

LOG_LEVEL FileLog::GetLogLevel() const {
	lock_guard<mutex> lock(this->mutexForSettings);
	return this->logLevel;
}

void FileLog::SetLogLevel(const LOG_LEVEL &logLevel) {
	lock_guard<mutex> lock(this->mutexForSettings);
	this->logLevel = logLevel;
}

string FileLog::GetOutputPath() const {
	lock_guard<mutex> lock(this->mutexForSettings);
	return this->outputPath;
}

void FileLog::SetOutputPath(const string &outputPath) {
	lock_guard<mutex> lock(this->mutexForSettings);
	this->outputPath = outputPath;
}

string FileLog::GetFileName() const {
	lock_guard<mutex> lock(this->mutexForSettings);
	return this->fileName;
}

void FileLog::SetFileName(const string &fileName) {
	lock_guard<mutex> lock(this->mutexForSettings);
	this->fileName = fileName;
}

bool FileLog::GetLinePrint() const { return this->linePrint; }

void FileLog::SetLinePrint(const bool &linePrint) { this->linePrint.store(linePrint); }

bool FileLog::GetThreadMode() const { return this->threadMode; }

void FileLog::SetThreadMode(const bool &threadMode) { this->threadMode.store(threadMode); }
