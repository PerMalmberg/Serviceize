// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <functional>
#include <chrono>

namespace serviceize {

class Serviceize
{
public:
	Serviceize();
	virtual ~Serviceize();

	enum ServiceStart {
		AUTO_START = SERVICE_AUTO_START,
		BOOT_START = SERVICE_BOOT_START,
		DEMAND_START = SERVICE_DEMAND_START, // Manual
		DISABLED = SERVICE_DISABLED,
		SYSTEM_START = SERVICE_SYSTEM_START
	};

	static const std::string USER_LOCAL_SERVICE;
	static const std::string USER_LOCAL_SYSTEM;

	bool InstallService(
		ServiceStart startType,
		const std::string& serviceName,
		const std::string& displayName,
		const std::vector<std::string> dependencies,
		const std::string& account,
		const std::string& password,
		const std::vector<std::string>& arguments ) const;

	bool UninstallService( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
	bool StopService( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
private:
	std::function<void( SC_HANDLE )> myServiceCloser = []( SC_HANDLE o ) {
		if( o != nullptr )
		{
			CloseServiceHandle( o );
		}
	};
};

}