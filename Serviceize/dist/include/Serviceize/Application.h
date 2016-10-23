// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>
#include "Serviceize/Serviceize.h"

namespace serviceize {

class Application
{
public:
	explicit Application();
	virtual ~Application();

	int Run();

	int RunAsService();

protected:
	// Method called once the application is initialized, possible as a service.
	virtual int Main() = 0;


	bool IsService() const
	{
		return false;
	}

	// TODO: Signals, Control-C
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683242.aspx

protected:
	Serviceize myServiceizer;
};

}