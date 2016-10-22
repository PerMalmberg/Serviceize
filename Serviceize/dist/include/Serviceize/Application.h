// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>

class Application
{
public:
	explicit Application( const std::string& workingDirectory );
	virtual ~Application();

	int Run();

	int RunAsService();

protected:
	// Method called once the application is initialized, possible as a daemon.
	virtual int Main() = 0;
	const std::string myWorkingDirectory;


	bool IsDaemon() const
	{
		return false;
	}

};

