#pragma once

#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

class FileManager {
	private:
		FileManager() = default;
		virtual ~FileManager() = default;

	public:
		bool IsExist(const string &path) const;
		bool IsRegularFile(const string &path) const;
		bool IsDirectory(const string &path) const;

		int LockBetweenProcess(const string &path,
							   const mode_t &mode = 0755) const;
		bool LockBetweenProcess(const int &fd) const;
		bool UnLockBetweenProcess(const int &fd) const;

		tuple<bool, string> Read(const string &path) const;
		bool Write(const string &path, const string &data,
				   const ios_base::openmode &openMode) const;

		bool MakeDir(const string &path) const;
		bool MakeDirs(const string &path) const;

		bool Copy(const string &fromPath, const string &toPath,
				  filesystem::copy_options options =
					  filesystem::copy_options::none) const;
		bool CopyAll(const string &fromPath, const string &toPath) const;

		bool Remove(const string &path) const;
		bool RemoveAll(const string &path) const;

		string ToAbsolutePath(const string &path) const;
		string ToCanonicalPath(const string &path) const;
		string ToRelativePathToRootPath(const string &path) const;

		string GetTempPath() const;
		string GetRootPath(const string &path) const;
		string GetRelativePath(const string &path) const;
		vector<string> GetSubDirectories(const string &path) const;
		vector<string> GetRecursiveSubDirectories(const string &path) const;

		string GetCurrentPath() const;
		bool SetCurrentPath(const string &path) const;

		static FileManager &Instance();
};
