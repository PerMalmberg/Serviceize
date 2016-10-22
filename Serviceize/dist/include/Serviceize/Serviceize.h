// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once
#include <string>

class Serviceize
{
public:
	Serviceize();
	virtual ~Serviceize();

	bool Install( const std::string& serviceName, const std::string& displayName );
};

