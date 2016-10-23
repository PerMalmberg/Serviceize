// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include <Serviceize/Process.h>

namespace serviceize {

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
	if( myProc.hProcess != nullptr )
	{
		CloseHandle( myProc.hProcess );
	}

	if( myProc.hThread != nullptr )
	{
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
	for( auto& a : myArguments )
	{
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

	if( res == 0 )
	{
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

	if( waitRes == WAIT_ABANDONED )
	{
		result = true;
	}
	else if( waitRes == WAIT_TIMEOUT )
	{
		// Timeout, wait failed
	}
	else if( waitRes == WAIT_FAILED )
	{
		myLastError = GetLastError();
		if( myLastError == 0 )
		{
			result = true;
		}
	}
	else if( waitRes == WAIT_OBJECT_0 )
	{
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
	if( res )
	{
		exitCode = static_cast<int>(code);
	}

	if( !res )
	{
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
	if( GetExitCode( exitCode ) )
	{
		res = exitCode == STILL_ACTIVE;
	}

	return res;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
std::string Process::GetExecutableFullPath()
{
	TCHAR path[MAX_PATH];
	GetModuleFileName( NULL, path, MAX_PATH );
	return Process::FromWinAPI( path );
}

#ifdef UNICODE
///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
std::string Process::FromWinAPI( const wchar_t* s )
{
	// Determine number of bytes needed
	auto bytesNeeded = WideCharToMultiByte( CP_UTF8, 0, s, -1, nullptr, 0, nullptr, nullptr );
	auto utf8 = std::make_unique<char[]>( bytesNeeded );
	// Perform conversion
	auto bytesWritten = WideCharToMultiByte( CP_UTF8, 0, s, -1, utf8.get(), bytesNeeded, nullptr, nullptr );
	return std::string( utf8.get() );
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<wchar_t[]> Process::ToWinAPI( const std::string& s )
{
	// Determine how many wchar_t characters we'll need
	auto countRequired = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, nullptr, 0 );
	auto wBuff = std::make_unique<wchar_t[]>( countRequired );
	// Perform conversion
	auto countWritten = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, wBuff.get(), countRequired );
	return wBuff;
}
#else
std::string  Process::FromWinAPI( const char* s )
{
	return std::string( s );
}

std::unique_ptr<char[]> Process::ToWinAPI( const std::string& s )
{
	auto buff = std::make_unique<char[]>( s.length() + 1 );
	ZeroMemory( buff.get(), s.length() + 1 );
	CopyMemory( buff.get(), s.c_str(), s.length() );
	return buff;
}
#endif

}