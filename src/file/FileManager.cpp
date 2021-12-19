#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <functional>
using namespace std;

#include "FileManager.h"

template <class T>
static T Run(const function<T(error_code &)> &func);

template <class T>
static T Run(const function<T(const string &, error_code &)> &func, const string &strPath);

template <class T>
static T Run(const function<T(const string &, const string &, error_code &)> &func, const string &strFromPath, const string &strToPath);

template <class T>
static T Run(const function<T(error_code &)> &func)
{
	error_code errorCode;
	errorCode.clear();

	const T result = func(errorCode);

	errno = errorCode.value();

	return result;
}

template <class T>
static T Run(const function<T(const string &, error_code &)> &func, const string &strPath)
{
	error_code errorCode;
	errorCode.clear();

	const T result = func(strPath, errorCode);

	errno = errorCode.value();

	return result;
}

template <class T>
static T Run(const function<T(const string &, const string &, error_code &)> &func, const string &strFromPath, const string &strToPath)
{
	error_code errorCode;
	errorCode.clear();

	const T result = func(strFromPath, strToPath, errorCode);

	errno = errorCode.value();

	return result;
}

bool FileManager::IsExist(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::exists(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

bool FileManager::IsRegularFile(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::is_regular_file(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

bool FileManager::IsDirectory(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::is_directory(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

int FileManager::LockBetweenProcess(const string &strPath)
{
	const mode_t mode = 0775;
	const int iFD = open(strPath.c_str(), O_CREAT | O_RDWR, mode);
	if(iFD < 0) {
		return -1;
	}

	flock sFlock;
	sFlock.l_type = F_RDLCK | F_WRLCK;
	sFlock.l_start = 0;
	sFlock.l_whence = SEEK_SET;
	sFlock.l_len = 0;

	if(fcntl(iFD, F_SETLK, &sFlock) == -1) {
		close(iFD);

		return -1;
	}

	return iFD;
}

bool FileManager::LockBetweenProcess(const int &iFD)
{
	flock sFlock;
	sFlock.l_type = F_RDLCK | F_WRLCK;
	sFlock.l_start = 0;
	sFlock.l_whence = SEEK_SET;
	sFlock.l_len = 0;

	if(fcntl(iFD, F_SETLK, &sFlock) == -1) {
		return false;
	}

	return true;
}

bool FileManager::UnLockBetweenProcess(const int &iFD)
{
	flock sFlock;
	sFlock.l_type = F_UNLCK;
	sFlock.l_start = 0;
	sFlock.l_whence = SEEK_SET;
	sFlock.l_len = 0;

	if(fcntl(iFD, F_SETLK, &sFlock) == -1) {
		return false;
	}

	return true;
}

bool FileManager::Read(const string &strPath, string &strResult)
{
	strResult.clear();

	ifstream ifs;

	ifs.open(strPath);

	if(ifs.is_open() == false) {
		return false;
	}

	ifs.seekg(0, ios::end);

	const int iSize = ifs.tellg();

	strResult.resize(iSize);

	ifs.seekg(0, ios::beg);

	ifs.read(&strResult[0], iSize);

	return true;
}

bool FileManager::Write(const string &strPath, const string &strData, const ios_base::openmode &openMode)
{
	ofstream ofs;

	ofs.open(strPath, openMode);

	if(ofs.is_open() == false) {
		return false;
	}

	ofs << strData;

	ofs.close();

	return true;
}

bool FileManager::MakeDir(const string &strPath)
{
	if(this->IsExist(strPath) && this->IsDirectory(strPath)) {
		return true;
	}

	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::create_directory(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

bool FileManager::MakeDirs(const string &strPath)
{
	if(this->IsExist(strPath) && this->IsDirectory(strPath)) {
		return true;
	}

	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::create_directories(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

bool FileManager::Copy(const string &strFromPath, const string &strToPath)
{
	auto job = [&](const string &strFromPath, const string &strToPath, error_code &errorCode) -> auto {
					filesystem::copy(filesystem::path(strFromPath), filesystem::path(strToPath), errorCode);
					return !errorCode;
				};

	return Run<bool>(job, strFromPath, strToPath);
}

bool FileManager::Copy(const string &strFromPath, const string &strToPath, filesystem::copy_options options)
{
	error_code errorCode;
	errorCode.clear();

	filesystem::copy(filesystem::path(strFromPath), filesystem::path(strToPath), options, errorCode);

	errno = errorCode.value();

	return !errorCode;
}

bool FileManager::CopyAll(const string &strFromPath, const string &strToPath)
{
	auto job = [&](const string &strFromPath, const string &strToPath, error_code &errorCode) -> auto {
					filesystem::copy(filesystem::path(strFromPath), filesystem::path(strToPath), filesystem::copy_options::recursive, errorCode);

					return !errorCode;
				};

	return Run<bool>(job, strFromPath, strToPath);
}

bool FileManager::Remove(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::remove(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

bool FileManager::RemoveAll(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::remove_all(filesystem::path(strPath), errorCode);
				};

	return Run<bool>(job, strPath);
}

string FileManager::ToAbsolutePath(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::absolute(filesystem::path(strPath), errorCode);
				};

	return Run<string>(job, strPath);
}

string FileManager::ToCanonicalPath(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::canonical(filesystem::path(strPath), errorCode);
				};

	return Run<string>(job, strPath);
}

string FileManager::ToRelativePathToRootPath(const string &strPath)
{
	return filesystem::path(strPath).relative_path();
}

string FileManager::GetTempPath()
{
	auto job = [&](error_code &errorCode) -> auto {
					return filesystem::temp_directory_path(errorCode);
				};

	return Run<string>(job);
}

string FileManager::GetRootPath(const string &strPath)
{
	return filesystem::path(strPath).root_directory();
}

string FileManager::GetRelativePath(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					return filesystem::relative(filesystem::path(strPath), errorCode);
				};

	return Run<string>(job, strPath);
}

vector<string> FileManager::GetPathList(const string &strPath)
{
	vector<string> vecPath;
	vecPath.clear();

	for(const filesystem::directory_entry &iter : filesystem::directory_iterator(strPath)) {
		vecPath.push_back(iter.path());
	}

	return vecPath;
}

vector<string> FileManager::GetRecursivePathList(const string &strPath)
{
	vector<string> vecPath;
	vecPath.clear();

	for(const filesystem::directory_entry &iter : filesystem::recursive_directory_iterator(strPath)) {
		vecPath.push_back(iter.path());
	}

	return vecPath;
}

string FileManager::GetCurrentPath()
{
	auto job = [&](error_code &errorCode) -> auto {
					return filesystem::current_path(errorCode);
				};

	return Run<string>(job);
}

bool FileManager::SetCurrentPath(const string &strPath)
{
	auto job = [&](const string &strPath, error_code &errorCode) -> auto {
					filesystem::current_path(filesystem::path(strPath), errorCode);

					return !errorCode;
				};

	return Run<bool>(job, strPath);
}
