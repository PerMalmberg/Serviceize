// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include "targetver.h"

#define CATCH_CONFIG_RUNNER
#include "Catch/include/catch.hpp"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main( int argc, const char* argv[] )
{
	int result = 0;

	if( argc > 1 ) {
		if( strstr( argv[1], "sleep2" ) ) {
			std::this_thread::sleep_for( 2s );
			result = 2;
		}
	}
	else {
		result = result = Catch::Session().run( argc, argv );
	}
	

	return result;
}