#pragma once

#include <filesystem>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using namespace std;

class FileManager {
	private:
		FileManager() = default;
		virtual ~FileManager() = default;

	public:
		pair<bool, error_code> IsExist(const filesystem::path &path) const;

		pair<bool, error_code> IsRegularFile(const filesystem::path &path) const;

		pair<bool, error_code> IsDirectory(const filesystem::path &path) const;

		pair<int, error_code> LockBetweenProcess(const filesystem::path &path,
												 const mode_t &mode = 0755) const;
		error_code LockBetweenProcess(const int &fd) const;
		error_code UnLockBetweenProcess(const int &fd) const;

		pair<string, error_code> Read(const filesystem::path &path) const;
		error_code Write(const filesystem::path &path, const string &data,
						 const ios_base::openmode &openMode) const;

		pair<bool, error_code> CreateDirectory(const filesystem::path &path) const;
		pair<bool, error_code> CreateDirectories(const filesystem::path &path) const;

		error_code Copy(const filesystem::path &fromPath, const filesystem::path &toPath,
						filesystem::copy_options options = filesystem::copy_options::none) const;
		error_code CopyAll(const filesystem::path &fromPath, const filesystem::path &toPath) const;

		pair<bool, error_code> Remove(const filesystem::path &path) const;
		pair<bool, error_code> RemoveAll(const filesystem::path &path) const;

		pair<filesystem::path, error_code> ToAbsolutePath(const filesystem::path &path) const;

		pair<filesystem::path, error_code> ToCanonicalPath(const filesystem::path &path) const;

		filesystem::path ToRelativePath(const filesystem::path &path) const;

		pair<filesystem::path, error_code> GetTempPath() const;

		filesystem::path GetRootPath(const filesystem::path &path) const;

		pair<filesystem::path, error_code> GetRelativePath(const filesystem::path &path) const;

		vector<filesystem::path> GetSubDirectories(const filesystem::path &path) const;

		vector<filesystem::path> GetRecursiveSubDirectories(const filesystem::path &path) const;

		pair<filesystem::path, error_code> GetCurrentPath() const;
		error_code SetCurrentPath(const filesystem::path &path) const;

		static FileManager &Instance();
};
