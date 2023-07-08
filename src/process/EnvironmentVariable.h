#pragma once

#include <string>
#include <vector>

using namespace std;

class EnvironmentVariable {
	private:
		bool standAlone = false;
		string configPath = "";
		string binaryName = "";

		bool InitializeOptions(const vector<string> &args);
		bool InitializeCurrentPath() const;

	public:
		EnvironmentVariable() = default;
		virtual ~EnvironmentVariable() = default;

		bool Initialize(int argc, char *argv[]);
		bool Initialize(const vector<string> &args);

		string Usage() const;

		bool GetStandAlone() const;
		string GetConfigPath() const;
		string GetBinaryName() const;
};
