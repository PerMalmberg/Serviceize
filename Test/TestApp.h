#pragma once

#include <Serviceize/Application.h>

class TestApp :
	public Application
{
public:
	explicit TestApp( const std::string& workingDirectory );
	~TestApp();
	int Main() override;
};

