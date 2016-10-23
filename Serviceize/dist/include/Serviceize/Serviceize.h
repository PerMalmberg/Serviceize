// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace serviceize {

class Serviceize
{
public:
	Serviceize();
	virtual ~Serviceize();

	enum ServiceStart {
		// These values are from winnt.h, but we don't want to include winnt.h 
		// or windows.h in our header files if we can avoid it since it badly 
		// polutes the global namepace.
		BOOT_START = 0x0,
		SYSTEM_START = 0x1,
		AUTO_START = 0x2,
		DEMAND_START = 0x3, // Manual
		DISABLED = 0x4
	};

	static const std::string USER_LOCAL_SERVICE;
	static const std::string USER_LOCAL_SYSTEM;

	bool Install(
		ServiceStart startType,
		const std::string& serviceName,
		const std::string& displayName,
		const std::vector<std::string> dependencies,
		const std::string& account,
		const std::string& password,
		const std::vector<std::string>& arguments ) const;

	bool Uninstall( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
	bool Stop( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
	bool Start( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
private:
	
};

}