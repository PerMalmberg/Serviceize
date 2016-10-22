// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <functional>
#include <chrono>

class Application
{
public:
	explicit Application( const std::string& workingDirectory );
	virtual ~Application();

	int Run();

	int RunAsService();

	bool InstallService( 
		DWORD startType, // SERVICE_AUTO_START, SERVICE_BOOT_START, SERVICE_DEMAND_START, SERVICE_DISABLED, SERVICE_SYSTEM_START
		const std::string& serviceName,
		const std::string& displayName,
		const std::vector<std::string> dependencies,
		const std::string& account,
		const std::string& password,
		const std::vector<std::string>& arguments ) const;

	bool UninstallService( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;
	bool StopService( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const;

protected:
	// Method called once the application is initialized, possible as a daemon.
	virtual int Main() = 0;
	const std::string myWorkingDirectory;


	bool IsDaemon() const
	{
		return false;
	}

private:
	std::function<void(SC_HANDLE)> myServiceCloser = []( SC_HANDLE o ) {
		if( o != nullptr ) {
			CloseServiceHandle( o );
		}
	};
};

