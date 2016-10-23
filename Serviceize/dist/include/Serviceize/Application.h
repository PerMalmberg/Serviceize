// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <memory>
#include "Serviceize/ServiceController.h"

// TODO: Signals, Control-C
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms683242.aspx

namespace serviceize {


class Application
{
public:
	Application( const std::string& serviceName, bool canStop = true, bool canShutdown = true, bool canPause = true );
	virtual ~Application();

	static bool RunService( Application& app );
	static int Application::RunConsole( Application& app, int argc, const char* argv[] );

protected:
	// Method called once the application is initialized, possible as a service.
	virtual int Main() = 0;
	bool IsService() const { return myIsService; }

#ifdef UNICODE
	wchar_t* GetName() const { return myName.get(); }
#else
	char* GetName() const { return myName.get(); }
#endif

protected:
	ServiceController myServiceizer;

	// When implemented in a derived class, executes when a Start command is  
	// sent to the service by the SCM or when the operating system starts  
	// (for a service that starts automatically). Specifies actions to take  
	// when the service starts. 
	virtual void OnStart( std::vector<std::string>& arguments ) {}

	// When implemented in a derived class, executes when a Stop command is  
	// sent to the service by the SCM. Specifies actions to take when a  
	// service stops running. 
	virtual void OnStop() {}

	// When implemented in a derived class, executes when a Pause command is  
	// sent to the service by the SCM. Specifies actions to take when a  
	// service pauses. 
	virtual void OnPause() {}

	// When implemented in a derived class, OnContinue runs when a Continue  
	// command is sent to the service by the SCM. Specifies actions to take  
	// when a service resumes normal functioning after being paused. 
	virtual void OnContinue() {}

	// When implemented in a derived class, executes when the system is  
	// shutting down. Specifies what should occur immediately prior to the  
	// system shutting down. 
	virtual void OnShutdown() {}

	// User-defined code recevied
	virtual void OnControlCode( int code );

	// Method called when the application is run as a service.
	// Do not block - user a worker thread or similar mechanism
	// to perform long-running work.
	virtual void RunAsService() = 0;

	// Method called when the application is run from the console.
	// May block until application shall terminate.
	virtual int RunAsConsole() = 0;

private:
#ifdef UNICODE
	std::unique_ptr<wchar_t[]> myName;
	static void WINAPI ServiceMain( DWORD argc, PWSTR* argv );
	// Start the service. 
	void Start( DWORD argc, PWSTR *argv );
#else
	std::unique_ptr<char[]> myName;
	static void WINAPI ServiceMain( DWORD argc, PSTR* argv );
	// Start the service. 
	void Start( DWORD argc, const PSTR* argv);
#endif // UNICODE

	// This function is called by the SCM whenever a control code is sent to the service. 
	static void WINAPI ServiceCtrlHandler( DWORD ctrl );

	// This function is called whenever a control code received from the command prompt
	static BOOL WINAPI ConsoleSignalRoutine( DWORD control );

	void SetStatus( DWORD currentState,	DWORD exitCode = NO_ERROR, DWORD waitHint = 0 );

	static Application* myInstance;
	SERVICE_STATUS_HANDLE myStatusHandle;
	SERVICE_STATUS myStatus;
	bool myIsService = true;	

	// Stop the service
	void Stop();

	// Pause the service. 
	void Pause();

	// Resume the service after being paused. 
	void Continue();

	// Execute when the system is shutting down. 
	void Shutdown();
};

}