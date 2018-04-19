#pragma once

#include <Serviceize/Application.h>

class TestApp :
	public serviceize::Application
{
public:
	TestApp( bool enableManualTest = false );
	TestApp( int argc, const char* argv[], bool enableManualTest = false );
	~TestApp();

	bool InstallService();
	bool UninstallService();
	bool Start();
	bool Stop();

	void RunAsService() override;

	int RunAsConsole() override;

	void OnStart( std::vector<std::string>& arguments ) override;

	void OnStop() override;
private:
	int myReturnValue = 666;
	bool myTerminateManualTest;
	std::vector<std::string> myArguments;
};

