#include <cstdarg>
#include <cstring>
#include <iomanip>
using namespace std;

#include "FileManager.h"

#include "FileLog.h"

FileLog::FileLog()
	: eLogLevel(E_LOG_LEVEL::DEBUG), strFileName(""), strOutputPath(""),
	  strFileNamePrefix(""), bThread(false), uniqptrThreadPool(nullptr) {
	this->vecFuture.clear();
}

FileLog::~FileLog() { this->Finalize(); }

bool FileLog::Initialize(const E_LOG_LEVEL& eLogLevel, const string& strOutputPath,
						 const string& strFileNamePrefix, const bool& bThread) {
	this->Finalize();

	this->eLogLevel = eLogLevel;
	this->strOutputPath = strOutputPath;
	this->strFileNamePrefix = strFileNamePrefix;

	if (this->strOutputPath.size()) {
		if (FileManager().MakeDirs(this->strOutputPath) == false) {
			return false;
		}
	}

	this->SetThread(bThread);

	return true;
}

bool FileLog::Finalize() {
	this->SetThread(false);

	this->eLogLevel = E_LOG_LEVEL::DEBUG;
	this->strOutputPath = "";
	this->strFileNamePrefix = "";

	return true;
}

bool FileLog::Flush() {
	lock_guard<mutex> lock(this->mutexLock);

	for (auto& iter : this->vecFuture) {
		if (iter.valid()) {
			iter.get();
		}
	}

	this->vecFuture.clear();

	return true;
}

bool FileLog::IsPrint(const E_LOG_LEVEL& eLogLevel) {
	if (static_cast<underlying_type_t<E_LOG_LEVEL>>(eLogLevel) >=
		static_cast<underlying_type_t<E_LOG_LEVEL>>(this->eLogLevel)) {
		return true;
	}

	return false;
}

string FileLog::MakeFileName(const time_t& sTime) {
	tm sTm;
	if (localtime_r(&sTime, &sTm) == nullptr) {
		return "";
	}

	stringstream ss;
	ss << put_time(&sTm, "%Y%m%d.log");

	string strFileName = "";

	if (this->strOutputPath.size()) {
		strFileName += this->strOutputPath + "/";
	}

	if (this->strFileNamePrefix.empty()) {
		strFileName += ss.str();
	} else {
		strFileName += this->strFileNamePrefix + "_" + ss.str();
	}

	return strFileName;
}

string FileLog::MakePrefixLog(const time_t& sTime, const E_LOG_LEVEL& eLogLevel) {
	tm sTm;
	if (localtime_r(&sTime, &sTm) == nullptr) {
		return "";
	}

	stringstream ss;
	ss << put_time(&sTm, "%H:%M:%S");

	char caPrefix[1024];
	memset(caPrefix, 0x00, sizeof(caPrefix));

	snprintf(caPrefix, sizeof(caPrefix) - 1, "[%s, %s] : ", ss.str().c_str(),
			 GmapLogLevelInfo.at(eLogLevel).c_str());

	return caPrefix;
}

bool FileLog::Logging(const E_LOG_LEVEL& eLogLevel, const string& strFormat, ...) {
	if (this->IsPrint(eLogLevel) == false) {
		return true;
	}

	va_list ap;

	va_start(ap, strFormat);

	char* pcStr = nullptr;
	vasprintf(&pcStr, strFormat.c_str(), ap);

	va_end(ap);

	const time_t sTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
	const string strLog = this->MakePrefixLog(sTime, eLogLevel) + " " +
						  (pcStr && pcStr[0] ? pcStr : "") + "\r\n";
	const string strFileName = this->MakeFileName(sTime);

	auto job = [=](const string& strLog, const string& strFileName) -> bool {
		if (this->strOutputPath.empty()) {
			printf("%s", strLog.c_str());
			return true;
		}

		return FileManager().Write(strFileName, strLog, ios::app);
	};

	if (this->bThread && this->uniqptrThreadPool.get()) {
		lock_guard<mutex> lock(this->mutexLock);
		this->vecFuture.push_back(
			move(this->uniqptrThreadPool->AddJob(job, strLog, strFileName)));
	} else {
		job(strLog, strFileName);
	}

	if (this->vecFuture.size() > 1000) {
		this->Flush();
	}

	return true;
}

bool FileLog::LoggingWithSourceLocation(const E_LOG_LEVEL& eLogLevel,
										const string& strFileName, const int& iFileLine,
										const string& strFormat, ...) {
	va_list ap;

	va_start(ap, strFormat);

	char* pcStr = nullptr;
	vasprintf(&pcStr, strFormat.c_str(), ap);

	va_end(ap);

	const string strPrefixFormat = pcStr && pcStr[0] ? pcStr : "";
	const string strPostfixFormat = "(%s:%d)";
	const string strFormatFinal = strPrefixFormat.size()
									  ? strPrefixFormat + " " + strPostfixFormat
									  : strPostfixFormat;

	return this->Logging(eLogLevel, strFormatFinal, strFileName.c_str(), iFileLine);
}

void FileLog::SetThread(const bool& bThread) {
	this->bThread.store(bThread);

	if (this->bThread && this->uniqptrThreadPool.get() == nullptr) {
		this->uniqptrThreadPool = make_unique<ThreadPool>(1);
	} else if (this->bThread == false) {
		this->Flush();
		this->uniqptrThreadPool.reset(nullptr);
	}
}
