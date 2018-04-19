#include "TestApp.h"
#include <thread>
#include <fstream>

using namespace std::chrono_literals;
using namespace serviceize;

TestApp::TestApp( bool enableManualTest )
	: TestApp( 0, nullptr, enableManualTest)
{

}

TestApp::TestApp( int argc, const char* argv[], bool enableManualTest )
	: Application( "TestService", argc, argv ), myTerminateManualTest( !enableManualTest ), myArguments()
{
}


TestApp::~TestApp()
{
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
		std::vector<std::string>{ "--runservice" } );
}

bool TestApp::UninstallService()
{
	// Uninstall ourselves
	return myServiceizer.Uninstall( "TestService", 3s );
}

bool TestApp::Start()
{
	return myServiceizer.Start( "TestService" );
}

bool TestApp::Stop()
{
	return myServiceizer.Stop( "TestService", 3s );
}

void TestApp::OnStart( std::vector<std::string>& arguments )
{
	if( arguments.size() == 2 )
	{
		if( arguments[0] == "Foo" )
		{
			myReturnValue = 5;
		}
	}

	for( auto& a : arguments )
	{
		myArguments.push_back( a );
	}
}

void TestApp::RunAsService()
{
	std::ofstream f( "d:\\temp\\arg.txt", std::ofstream::out | std::ofstream::binary );
	for( auto& s : myArguments )
	{
		f << s << std::endl;
	}
	f.close();

}

int TestApp::RunAsConsole()
{
	while( !myTerminateManualTest )
	{
		std::this_thread::sleep_for( 40ms );
	}
	return myReturnValue;
}

void TestApp::OnStop()
{
	// This is for the manual commandline test (run with --commandline and press CTRL-C to test)
	myTerminateManualTest = true;
}