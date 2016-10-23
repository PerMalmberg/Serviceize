#pragma once

#include <Serviceize/Application.h>

class TestApp :
	public serviceize::Application
{
public:
	TestApp();
	~TestApp();
	int Main() override;

	bool InstallService();
	bool UninstallService();
	bool Start();
	bool Stop();
};

