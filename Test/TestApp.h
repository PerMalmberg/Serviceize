#pragma once

#include <Serviceize/Application.h>

class TestApp :
	public serviceize::Application
{
public:
	explicit TestApp( const std::string& workingDirectory );
	~TestApp();
	int Main() override;

	bool InstallService();
	bool UninstallService();
};

