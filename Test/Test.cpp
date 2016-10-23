// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include "Catch/include/catch.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <Serviceize\Process.h>
#include "TestApp.h"

using namespace std::chrono_literals;
using namespace serviceize;

SCENARIO( "Can run test project with arguments" )
{
	GIVEN( "A process object" )
	{
		WHEN( "Setup with sleep2 argument" )
		{
			Process proc{
				Process::GetExecutableFullPath(),
				std::vector<std::string>{ "sleep2" }
			};

			THEN( "When executed" )
			{
				REQUIRE( proc.Execute() );
				REQUIRE( 0 == proc.GetErrorCode() );
				REQUIRE_FALSE( proc.WaitForTermination( 500ms ) );
				REQUIRE( proc.IsStillActive() );
				int exitCode;
				REQUIRE( proc.WaitForTermination( 2s ) );
				REQUIRE( proc.GetExitCode( exitCode ) );
				REQUIRE( exitCode == 2 );
			}
		}
	}
}

SCENARIO( "File to execute doesn't exist" )
{
	GIVEN( "A process object" )
	{
		WHEN( "Setup with bad command file name " )
		{
			Process proc{ "FileDoesNotExist" };

			THEN( "When executed, error is file not found" )
			{
				REQUIRE_FALSE( proc.Execute() );
				REQUIRE( proc.GetErrorCode() == ERROR_FILE_NOT_FOUND );
			}
		}
	}
}

SCENARIO( "Installing and uninstalling service" )
{
	GIVEN( "An application object" )
	{
		TestApp app;

		WHEN( "Installing service" )
		{
			THEN( "Service is installed" )
			{
				REQUIRE( app.InstallService() );
			}
			AND_THEN( "Service is started" )
			{
				REQUIRE( app.Start() );
			}
			AND_THEN( "Service is stopped" )
			{
				REQUIRE( app.Stop() );
			}
			AND_THEN( "Service is uninstalled" )
			{
				REQUIRE( app.UninstallService() );
			}
		}
	}
}

