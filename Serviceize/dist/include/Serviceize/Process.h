// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <chrono>
#include <memory>

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
	bool IsStillActive() const;

#ifdef UNICODE

	static std::string FromWinAPI( const wchar_t* s )
	{
		// Determine number of bytes needed
		auto bytesNeeded = WideCharToMultiByte( CP_UTF8, 0, s, -1, nullptr, 0, nullptr, nullptr );
		auto utf8 = std::make_unique<char[]>( bytesNeeded );
		// Perform conversion
		auto bytesWritten = WideCharToMultiByte( CP_UTF8, 0, s, -1, utf8.get(), bytesNeeded, nullptr, nullptr );
		return std::string( utf8.get() );
	}

	static std::unique_ptr<wchar_t[]> ToWinAPI( const std::string& s )
	{
		// Determine how many wchar_t characters we'll need
		auto countRequired = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, nullptr, 0 );
		auto wBuff = std::make_unique<wchar_t[]>( countRequired );
		// Perform conversion
		auto countWritten = MultiByteToWideChar( CP_UTF8, 0, s.c_str(), -1, wBuff.get(), countRequired );
		return wBuff;
	}

#else

	static std::string FromWinAPI( const char* s )
	{
		return std::string( s );
	}

	static std::unique_ptr<char[]> ToWinAPI( const std::string& s )
	{
		auto buff = std::make_unique<char[]>( s.length() + 1 );
		ZeroMemory( buff.get(), s.length() + 1 );
		CopyMemory( buff.get(), s.c_str(), s.length() );
		return buff;
	}
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

