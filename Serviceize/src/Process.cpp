// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include <Serviceize/Process.h>

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
Process::Process( const std::string& commandFile )
	: Process( commandFile, std::vector<std::string>() )
{
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
Process::Process( const std::string& commandFile, std::vector<std::string>& arguments )
	: myCommandFile( commandFile ), myArguments( arguments )
{
	ZeroMemory( &myStart, sizeof( myStart ) );
	myStart.cb = sizeof( myStart );
	ZeroMemory( &myProc, sizeof( myProc ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
Process::~Process()
{
	if( myProc.hProcess != nullptr ) {
		CloseHandle( myProc.hProcess );
	}

	if( myProc.hThread != nullptr ) {
		CloseHandle( myProc.hThread );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
bool Process::Execute()
{
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682425(v=vs.85).aspx
	std::string args = myCommandFile;
	for( auto& a : myArguments ) {
		args += " " + Quote( a );
	}

	auto cmdFile = ToWinAPI( myCommandFile );
	auto cmdLine = ToWinAPI( args );

	myLastError = 0;

	auto res = CreateProcess( cmdFile.get(),   // No module name (use command line)
		cmdLine.get(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&myStart,       // Pointer to STARTUPINFO structure
		&myProc );		// Pointer to PROCESS_INFORMATION structure

	if( res == 0 ) {
		myLastError = GetLastError();
	}

	return res != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
bool Process::WaitForTermination( std::chrono::milliseconds milliSeconds )
{
	auto waitRes = WaitForSingleObject( myProc.hProcess, static_cast<DWORD>(milliSeconds.count()) );
	
	bool result = false;

	if( waitRes == WAIT_ABANDONED ) {
		result = true;
	}
	else if( waitRes == WAIT_TIMEOUT ) {
		// Timeout, wait failed
	}
	else if( waitRes == WAIT_FAILED ) {
		myLastError = GetLastError();
		if( myLastError == 0 ) {
			result = true;
		}
	}
	else if( waitRes == WAIT_OBJECT_0 ) {
		result = true;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
std::string Process::Quote( const std::string& s )
{
	return "\"" + s + "\"";
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
bool Process::GetExitCode( int& exitCode )
{
	DWORD code;
	auto res = GetExitCodeProcess( myProc.hProcess, &code );
	if( res ) {
		exitCode = static_cast<int>(code);
	}

	if( !res ) {
		myLastError = GetLastError();
	}

	return res != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
bool Process::IsStillActive()
{
	bool res = false;


	int exitCode;
	if( GetExitCode( exitCode ) ) {
		res = exitCode == STILL_ACTIVE;
	}
	
	return res;

}
