#include "FileManager.h"
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

using namespace std;

template <class T> static T run(const function<T(error_code &)> &func);

template <class T>
static T run(const function<T(const string &, error_code &)> &func, const string &path);

template <class T>
static T run(const function<T(const string &, const string &, error_code &)> &func,
			 const string &fromPath, const string &toPath);

template <class T> static T run(const function<T(error_code &)> &func) {
	error_code errorCode;
	errorCode.clear();

	const T result = func(errorCode);

	errno = errorCode.value();

	return result;
}

template <class T>
static T run(const function<T(const string &, error_code &)> &func, const string &path) {
	error_code errorCode;
	errorCode.clear();

	const T result = func(path, errorCode);

	errno = errorCode.value();

	return result;
}

template <class T>
static T run(const function<T(const string &, const string &, error_code &)> &func,
			 const string &fromPath, const string &toPath) {
	error_code errorCode;
	errorCode.clear();

	const T result = func(fromPath, toPath, errorCode);

	errno = errorCode.value();

	return result;
}

bool FileManager::IsExist(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::exists(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

bool FileManager::IsRegularFile(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::is_regular_file(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

bool FileManager::IsDirectory(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::is_directory(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

int FileManager::LockBetweenProcess(const string &path, const mode_t &mode) const {
	const int fd = open(path.c_str(), O_CREAT | O_RDWR, mode);
	if (fd < 0) {
		return -1;
	}

	if (this->LockBetweenProcess(fd) == false) {
		close(fd);

		return -1;
	}

	return fd;
}

bool FileManager::LockBetweenProcess(const int &fd) const {
	flock lock;
	lock.l_type = F_RDLCK | F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) == -1) {
		return false;
	}

	return true;
}

bool FileManager::UnLockBetweenProcess(const int &fd) const {
	flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) == -1) {
		return false;
	}

	return true;
}

tuple<bool, string> FileManager::Read(const string &path) const {
	ifstream ifs;

	ifs.open(path);
	if (ifs.is_open() == false) {
		return make_tuple(false, "");
	}

	ifs.seekg(0, ios::end);

	const int size = ifs.tellg();

	string result = "";
	result.resize(size);

	ifs.seekg(0, ios::beg);

	ifs.read(&result[0], size);

	return make_tuple(true, result);
}

bool FileManager::Write(const string &path, const string &data,
						const ios_base::openmode &openMode) const {
	ofstream ofs;

	ofs.open(path, openMode);
	if (ofs.is_open() == false) {
		return false;
	}

	ofs << data;

	ofs.close();

	return true;
}

bool FileManager::MakeDir(const string &path) const {
	if (this->IsExist(path) && this->IsDirectory(path)) {
		return true;
	}

	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::create_directory(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

bool FileManager::MakeDirs(const string &path) const {
	if (this->IsExist(path) && this->IsDirectory(path)) {
		return true;
	}

	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::create_directories(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

bool FileManager::Copy(const string &fromPath, const string &toPath,
					   filesystem::copy_options options) const {
	error_code errorCode;
	errorCode.clear();

	filesystem::copy(filesystem::path(fromPath), filesystem::path(toPath), options, errorCode);

	errno = errorCode.value();

	return !errorCode;
}

bool FileManager::CopyAll(const string &fromPath, const string &toPath) const {
	const auto job = [](const string &fromPath, const string &toPath, error_code &errorCode) {
		filesystem::copy(filesystem::path(fromPath), filesystem::path(toPath),
						 filesystem::copy_options::recursive, errorCode);

		return !errorCode;
	};

	return run<bool>(job, fromPath, toPath);
}

bool FileManager::Remove(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::remove(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

bool FileManager::RemoveAll(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::remove_all(filesystem::path(path), errorCode);
	};

	return run<bool>(job, path);
}

string FileManager::ToAbsolutePath(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::absolute(filesystem::path(path), errorCode);
	};

	return run<string>(job, path);
}

string FileManager::ToCanonicalPath(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::canonical(filesystem::path(path), errorCode);
	};

	return run<string>(job, path);
}

string FileManager::ToRelativePathToRootPath(const string &path) const {
	return filesystem::path(path).relative_path();
}

string FileManager::GetTempPath() const {
	const auto job = [](error_code &errorCode) {
		return filesystem::temp_directory_path(errorCode);
	};

	return run<string>(job);
}

string FileManager::GetRootPath(const string &path) const {
	return filesystem::path(path).root_directory();
}

string FileManager::GetRelativePath(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		return filesystem::relative(filesystem::path(path), errorCode);
	};

	return run<string>(job, path);
}

vector<string> FileManager::GetSubDirectories(const string &path) const {
	vector<string> paths;
	paths.clear();

	for (const filesystem::directory_entry &iter : filesystem::directory_iterator(path)) {
		paths.push_back(iter.path());
	}

	return paths;
}

vector<string> FileManager::GetRecursiveSubDirectories(const string &path) const {
	vector<string> paths;
	paths.clear();

	for (const filesystem::directory_entry &iter : filesystem::recursive_directory_iterator(path)) {
		paths.push_back(iter.path());
	}

	return paths;
}

string FileManager::GetCurrentPath() const {
	const auto job = [](error_code &errorCode) { return filesystem::current_path(errorCode); };

	return run<string>(job);
}

bool FileManager::SetCurrentPath(const string &path) const {
	const auto job = [](const string &path, error_code &errorCode) {
		filesystem::current_path(filesystem::path(path), errorCode);

		return !errorCode;
	};

	return run<bool>(job, path);
}

FileManager &FileManager::Instance() {
	static FileManager fileManager;
	return fileManager;
}
