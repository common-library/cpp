#include "FileManager.h"
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;

pair<bool, error_code> FileManager::IsExist(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::exists(path, errorCode);

	return make_pair(result, errorCode);
}

pair<bool, error_code> FileManager::IsRegularFile(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::is_regular_file(path, errorCode);

	return make_pair(result, errorCode);
}

pair<bool, error_code> FileManager::IsDirectory(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::is_directory(path, errorCode);

	return make_pair(result, errorCode);
}

pair<int, error_code> FileManager::LockBetweenProcess(const filesystem::path &path,
													  const mode_t &mode) const {
	const int fd = open(path.c_str(), O_CREAT | O_RDWR, mode);
	if (fd < 0) {
		return make_pair(-1, error_code(errno, system_category()));
	}

	if (error_code errorCode = this->LockBetweenProcess(fd); errorCode) {
		close(fd);
		return make_pair(-1, errorCode);
	}

	return make_pair(fd, error_code(0, system_category()));
}

error_code FileManager::LockBetweenProcess(const int &fd) const {
	flock lock;
	lock.l_type = F_RDLCK | F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) == -1) {
		return error_code(errno, system_category());
	}

	return error_code(0, system_category());
}

error_code FileManager::UnLockBetweenProcess(const int &fd) const {
	flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) == -1) {
		return error_code(errno, system_category());
	}

	return error_code(0, system_category());
}

pair<string, error_code> FileManager::Read(const filesystem::path &path) const {
	ifstream ifs;

	ifs.open(path);
	if (ifs.is_open() == false) {
		return make_pair("", error_code(errno, system_category()));
	}

	ifs.seekg(0, ios::end);

	const int size = ifs.tellg();

	string result = "";
	result.resize(size);

	ifs.seekg(0, ios::beg);

	ifs.read(&result[0], size);

	return make_pair(result, error_code(0, system_category()));
}

error_code FileManager::Write(const filesystem::path &path, const string &data,
							  const ios_base::openmode &openMode) const {
	ofstream ofs;

	ofs.open(path, openMode);
	if (ofs.is_open() == false) {
		return error_code(errno, system_category());
	}

	ofs << data;

	ofs.close();

	return error_code(0, system_category());
}

pair<bool, error_code> FileManager::CreateDirectory(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::create_directory(path, errorCode);

	return make_pair(result, errorCode);
}

pair<bool, error_code> FileManager::CreateDirectories(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::create_directories(path, errorCode);

	return make_pair(result, errorCode);
}

error_code FileManager::Copy(const filesystem::path &fromPath, const filesystem::path &toPath,
							 filesystem::copy_options options) const {
	error_code errorCode;

	filesystem::copy(fromPath, toPath, options, errorCode);

	return errorCode;
}

error_code FileManager::CopyAll(const filesystem::path &fromPath,
								const filesystem::path &toPath) const {
	error_code errorCode;

	filesystem::copy(fromPath, toPath, filesystem::copy_options::recursive, errorCode);

	return errorCode;
}

pair<bool, error_code> FileManager::Remove(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::remove(path, errorCode);

	return make_pair(result, errorCode);
}

pair<bool, error_code> FileManager::RemoveAll(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::remove_all(path, errorCode);

	return make_pair(result, errorCode);
}

pair<filesystem::path, error_code> FileManager::ToAbsolutePath(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::absolute(path, errorCode);

	return make_pair(result, errorCode);
}

pair<filesystem::path, error_code>
FileManager::ToCanonicalPath(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::canonical(path, errorCode);

	return make_pair(result, errorCode);
}

filesystem::path FileManager::ToRelativePath(const filesystem::path &path) const {
	return path.relative_path();
}

pair<filesystem::path, error_code> FileManager::GetTempPath() const {
	error_code errorCode;

	auto result = filesystem::temp_directory_path(errorCode);

	return make_pair(result, errorCode);
}

filesystem::path FileManager::GetRootPath(const filesystem::path &path) const {
	return path.root_directory();
}

pair<filesystem::path, error_code>
FileManager::GetRelativePath(const filesystem::path &path) const {
	error_code errorCode;

	auto result = filesystem::relative(path, errorCode);

	return make_pair(result, errorCode);
}

vector<filesystem::path> FileManager::GetSubDirectories(const filesystem::path &path) const {
	vector<filesystem::path> paths;
	paths.clear();

	for (const filesystem::directory_entry &iter : filesystem::directory_iterator(path)) {
		paths.push_back(iter.path());
	}

	return paths;
}

vector<filesystem::path>
FileManager::GetRecursiveSubDirectories(const filesystem::path &path) const {
	vector<filesystem::path> paths;
	paths.clear();

	for (const filesystem::directory_entry &iter : filesystem::recursive_directory_iterator(path)) {
		paths.push_back(iter.path());
	}

	return paths;
}

pair<filesystem::path, error_code> FileManager::GetCurrentPath() const {
	error_code errorCode;

	return make_pair(filesystem::current_path(errorCode), errorCode);
}

error_code FileManager::SetCurrentPath(const filesystem::path &path) const {
	error_code errorCode;

	filesystem::current_path(path, errorCode);

	return errorCode;
}

FileManager &FileManager::Instance() {
	static FileManager fileManager;
	return fileManager;
}
