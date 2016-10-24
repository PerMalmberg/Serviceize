// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include "targetver.h"
#include <chrono>
#include <thread>

#define CATCH_CONFIG_RUNNER
#include "Catch/include/catch.hpp"
#include "TestApp.h"

using namespace std::chrono_literals;

int main( int argc, const char* argv[] )
{
	int result = 1;

	if( argc > 1 )
	{
		if( strcmp( argv[1], "sleep2" ) == 0 )
		{
			std::this_thread::sleep_for( 2s );
			result = 2;
		}
		else if( strcmp( argv[1], "install" ) == 0 )
		{
			TestApp app;
			result = app.InstallService() ? 0 : 1;
			std::cout << "Installed: " << result << std::endl;
		}
		else if( strcmp( argv[1], "uninstall" ) == 0 )
		{
			TestApp app;
			result = app.UninstallService() ? 0 : 1;
			std::cout << "Uninstalled: " << result << std::endl;
		}
		else if( strcmp( argv[1], "start" ) == 0 )
		{
			TestApp app;
			result = app.Start() ? 0 : 1;
			std::cout << "Start: " << result << std::endl;
		}
		else if( strcmp( argv[1], "stop" ) == 0 )
		{
			TestApp app;
			result = app.Stop() ? 0 : 1;
			std::cout << "Stop: " << result << std::endl;
		}
		else if( strcmp( argv[1], "--runservice" ) == 0 )
		{
			// Run as a service
			TestApp app( argc, argv );
			result = TestApp::RunService( app ) ? 0 : 1;
		}
		else if( strcmp( argv[1], "--commandline" ) == 0 )
		{
			TestApp app( argc, argv );
			result = TestApp::RunConsole( app );
		}
	}
	else
	{
		result = result = Catch::Session().run( argc, argv );
	}


	return result;
}