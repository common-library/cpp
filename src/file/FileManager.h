#pragma once

#include <filesystem>
#include <vector>
using namespace std;

class FileManager {
	private:
	public:
		FileManager() = default;
		virtual ~FileManager() = default;

		bool IsExist(const string& strPath);
		bool IsRegularFile(const string& strPath);
		bool IsDirectory(const string& strPath);

		bool LockBetweenProcess(const int& iFD);
		int LockBetweenProcess(const string& strPath);
		bool UnLockBetweenProcess(const int& iFD);

		bool Read(const string& strPath, string& strResult);
		bool Write(const string& strPath, const string& strData,
				   const ios_base::openmode& openMode);

		bool MakeDir(const string& strPath);
		bool MakeDirs(const string& strPath);

		bool Copy(const string& strFromPath, const string& strToPath);
		bool Copy(const string& strFromPath, const string& strToPath,
				  filesystem::copy_options options);
		bool CopyAll(const string& strFromPath, const string& strToPath);

		bool Remove(const string& strPath);
		bool RemoveAll(const string& strPath);

		string ToAbsolutePath(const string& strPath);
		string ToCanonicalPath(const string& strPath);
		string ToRelativePathToRootPath(const string& strPath);

		string GetTempPath();
		string GetRootPath(const string& strPath);
		string GetRelativePath(const string& strPath);
		vector<string> GetPathList(const string& strPath);
		vector<string> GetRecursivePathList(const string& strPath);

		string GetCurrentPath();
		bool SetCurrentPath(const string& strPath);
};
