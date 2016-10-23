#include "TestApp.h"

using namespace std::chrono_literals;
using namespace serviceize;

TestApp::TestApp()
	: Application( "TestService" )
{
}


TestApp::~TestApp()
{
}

int TestApp::Main()
{
	return 1;
}

bool TestApp::InstallService()
{
	// Install ourselves as a service
	return myServiceizer.Install(
		ServiceController::DEMAND_START,
		"TestService",
		"TestServiceDisplayName",
		std::vector<std::string>{},
		ServiceController::USER_LOCAL_SERVICE,
		"",
		std::vector<std::string>{ "-runservice" } );
}

bool TestApp::UninstallService()
{
	// Uninstall ourselves
	return myServiceizer.Uninstall( "TestService", 3s );
}

bool TestApp::Start()
{
	return myServiceizer.Start( "TestService", 3s );
}

bool TestApp::Stop()
{
	return myServiceizer.Stop( "TestService", 3s );
}