

#include <strsafe.h>
#include <sstream>
#include "Serviceize/Application.h"
#include "Serviceize\Process.h"

namespace serviceize {

Application* Application::myInstance = nullptr;

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
Application::Application( const std::string& serviceName, bool canStop, bool canShutdown, bool canPause )
	:myName( Process::ToWinAPI( serviceName ) )
{	
	myStatusHandle = nullptr;

	// The service runs in its own process. 
	myStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	// The service is starting. 
	myStatus.dwCurrentState = SERVICE_START_PENDING;

	// The accepted commands of the service. 
	myStatus.dwControlsAccepted = canShutdown ? SERVICE_ACCEPT_SHUTDOWN : 0;
	myStatus.dwControlsAccepted |= canStop ? SERVICE_ACCEPT_STOP : 0;
	myStatus.dwControlsAccepted |= canPause ? SERVICE_ACCEPT_PAUSE_CONTINUE : 0;
	myStatus.dwWin32ExitCode = NO_ERROR;
	myStatus.dwServiceSpecificExitCode = 0;
	myStatus.dwCheckPoint = 0;
	myStatus.dwWaitHint = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
bool Application::RunService( Application& app )
{
	myInstance = &app;

	SERVICE_TABLE_ENTRY service[] =
	{
		{ app.GetName(), ServiceMain },
		{ NULL, NULL }
	};

	// Connects the main thread of a service process to the service control  
	// manager, which causes the thread to be the service control dispatcher  
	// thread for the calling process. This call returns when the service has  
	// stopped. The process should simply terminate when the call returns. 
	return StartServiceCtrlDispatcher( service ) == TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
void WINAPI Application::ServiceMain( DWORD argc, PWSTR *argv )
#else
void WINAPI Application::ServiceMain( DWORD argc, PSTR *argv )
#endif
{
	// Register the handler function for the service 
	myInstance->myStatusHandle = RegisterServiceCtrlHandler(
		myInstance->GetName(), ServiceCtrlHandler );

	if( myInstance->myStatusHandle == nullptr )
	{
		throw GetLastError();
	}

	// Start the service.
	myInstance->Start( argc, argv );
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
// The function is called by the SCM whenever a control code is sent to  
// the service. 
void WINAPI Application::ServiceCtrlHandler( DWORD ctrl )
{
	// SERVICE_CONTROL_CONTINUE
	// SERVICE_CONTROL_INTERROGATE 
	// SERVICE_CONTROL_NETBINDADD 
	// SERVICE_CONTROL_NETBINDDISABLE 
	// SERVICE_CONTROL_NETBINDREMOVE 
	// SERVICE_CONTROL_PARAMCHANGE 
	// SERVICE_CONTROL_PAUSE 
	// SERVICE_CONTROL_SHUTDOWN 
	// SERVICE_CONTROL_STOP
	// This parameter can also be a user defined control code ranges from 128 to 255

	if( ctrl == SERVICE_CONTROL_CONTINUE )
	{
		myInstance->Continue();
	}
	else if( ctrl == SERVICE_CONTROL_STOP )
	{
		myInstance->Stop();
	}
	else if( ctrl == SERVICE_CONTROL_PAUSE )
	{
		myInstance->Pause();
	}
	else if( ctrl == SERVICE_CONTROL_SHUTDOWN )
	{
		myInstance->Shutdown();
	}
	else if( ctrl >= 128 && ctrl <= 255 )
	{
		myInstance->ControlCode( ctrl );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// 
//
///////////////////////////////////////////////////////////////////////////////
void Application::SetStatus( DWORD currentState, DWORD exitCode, DWORD waitHint )
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure of the service. 

	myStatus.dwCurrentState = currentState;
	myStatus.dwWin32ExitCode = exitCode;
	myStatus.dwWaitHint = waitHint;

	// If we're running or stopped, don't report progress
	myStatus.dwCheckPoint = ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED)) ? 0 : dwCheckPoint++;

	// Report the status of the service to the SCM. 
	SetServiceStatus( myStatusHandle, &myStatus );
}

///////////////////////////////////////////////////////////////////////////////
//
// Start the service. 
//
///////////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
void Application::Start( DWORD argc, PWSTR *argv )
#else
void Application::Start( DWORD argc, PSTR *argv )
#endif
{
	try
	{
		// Tell SCM that the service is starting. 
		SetStatus( SERVICE_START_PENDING );

		std::vector<std::string> arguments;
		for( DWORD i = 0; i < argc; ++i )
		{
			arguments.push_back( Process::FromWinAPI( argv[i] ) );
		}

		// Perform service-specific initialization. 
		OnStart( arguments );

		// Tell SCM that the service is started. 
		SetStatus( SERVICE_RUNNING );
	}
	catch( DWORD error )
	{
		SetStatus( SERVICE_STOPPED, error );
	}
	catch( ... )
	{
		// Set the service status to be stopped. 
		SetStatus( SERVICE_STOPPED );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// 
//
///////////////////////////////////////////////////////////////////////////////
void Application::Stop()
{
	auto originalState = myStatus.dwCurrentState;
	try
	{
		// Tell SCM that the service is stopping. 
		SetStatus( SERVICE_STOP_PENDING );

		// Perform service-specific stop operations. 
		OnStop();

		// Tell SCM that the service is stopped. 
		SetStatus( SERVICE_STOPPED );
	}
	catch( DWORD )
	{
		SetStatus( originalState );
	}
	catch( ... )
	{
		SetStatus( originalState );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Pause the service. 
//
///////////////////////////////////////////////////////////////////////////////
void Application::Pause()
{
	try
	{
		// Tell SCM that the service is pausing. 
		SetStatus( SERVICE_PAUSE_PENDING );

		// Perform service-specific pause operations. 
		OnPause();

		// Tell SCM that the service is paused. 
		SetStatus( SERVICE_PAUSED );
	}
	catch( DWORD )
	{
		SetStatus( SERVICE_RUNNING );
	}
	catch( ... )
	{
		SetStatus( SERVICE_RUNNING );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Resume the service after being paused. 
//
///////////////////////////////////////////////////////////////////////////////
void Application::Continue()
{
	try
	{
		// Tell SCM that the service is resuming. 
		SetStatus( SERVICE_CONTINUE_PENDING );

		// Perform service-specific continue operations. 
		OnContinue();

		// Tell SCM that the service is running. 
		SetStatus( SERVICE_RUNNING );
	}
	catch( DWORD )
	{
		SetStatus( SERVICE_PAUSED );
	}
	catch( ... )
	{
		SetStatus( SERVICE_PAUSED );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Execute when the system is shutting down. 
//
///////////////////////////////////////////////////////////////////////////////
void Application::Shutdown()
{
	try
	{
		// Perform service-specific shutdown operations. 
		OnShutdown();

		// Tell SCM that the service is stopped. 
		SetStatus( SERVICE_STOPPED );
	}
	catch( DWORD )
	{

	}
	catch( ... )
	{

	}
}

///////////////////////////////////////////////////////////////////////////////
//
// User-defined code recevied
//
///////////////////////////////////////////////////////////////////////////////
void Application::ControlCode( int code )
{
	
}

/*
///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
int Application::Run()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
int Application::RunAsService()
{
	return 0;
}*/

}