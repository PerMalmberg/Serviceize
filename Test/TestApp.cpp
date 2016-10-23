#include "TestApp.h"

using namespace std::chrono_literals;
using namespace serviceize;

TestApp::TestApp()
	: Application()
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
		Serviceize::DEMAND_START,
		"TestService",
		"TestServiceDisplayName",
		std::vector<std::string>{},
		Serviceize::USER_LOCAL_SERVICE,
		"",
		std::vector<std::string>{} );
}

bool TestApp::UninstallService()
{
	// Install ourselves as a service
	return myServiceizer.Uninstall( "TestService", 3s );
}
