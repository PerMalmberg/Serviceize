// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#define VC_EXTRALEAN
#define WINODWS_LEAN_AND_MEAN
#include <windows.h>

namespace serviceize {

class Process
{
public:
	Process( const std::string& commandFile, std::vector<std::string>& arguments );
	explicit Process( const std::string& commandFile );

	virtual ~Process();

	bool Execute();

	bool WaitForTermination( std::chrono::milliseconds milliSeconds );

	DWORD GetErrorCode() const { return myLastError; }

	bool GetExitCode( int& exitCode );
	bool IsStillActive();
	static std::string GetExecutableFullPath();

#ifdef UNICODE
	static std::string FromWinAPI( const wchar_t* s );
	static std::unique_ptr<wchar_t[]> ToWinAPI( const std::string& s );
#else
	static std::string FromWinAPI( const char* s );
	static std::unique_ptr<char[]> ToWinAPI( const std::string& s );
#endif

protected:
	std::string Quote( const std::string& s );

private:
	std::string myCommandFile;
	std::vector<std::string> myArguments;
	STARTUPINFO myStart{};
	PROCESS_INFORMATION myProc{};
	DWORD myLastError = 0;
};

}