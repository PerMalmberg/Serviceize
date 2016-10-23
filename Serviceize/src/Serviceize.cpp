// Copyright (c) 2016 Per Malmberg
// Licensed under MIT, see LICENSE file.
// Give credit where credit is due.

#include <thread>
#include <Strsafe.h>
#include <memory>
#include <Serviceize/Serviceize.h>
#include "Serviceize/AutoCloser.h"
#include "Serviceize/Process.h"

using namespace std::chrono_literals;

#ifdef UNICODE
using CharType = wchar_t;
#else
using CharType = char;
#endif

namespace serviceize {

const std::string Serviceize::USER_LOCAL_SERVICE = "NT AUTHORITY\\LocalService";
const std::string Serviceize::USER_LOCAL_SYSTEM = "NT AUTHORITY\\LocalSystem";

std::function<void( SC_HANDLE )> myServiceCloser = []( SC_HANDLE o ) {
	if( o != nullptr )
	{
		CloseServiceHandle( o );
	}
};

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
Serviceize::Serviceize()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
Serviceize::~Serviceize()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
bool Serviceize::Install(
	ServiceStart startType,
	const std::string& serviceName,
	const std::string& displayName,
	const std::vector<std::string> dependencies,
	const std::string& account,
	const std::string& password,
	const std::vector<std::string>& arguments ) const
{
	// https://code.msdn.microsoft.com/windowsapps/CppWindowsService-cacf4948/sourcecode?fileId=21604&pathId=447141730

	bool result = false;


	// Administrative rights needed to create services
	AutoCloser<SC_HANDLE> manager( OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE ), myServiceCloser );

	if( manager.Get() != nullptr )
	{
		auto service = Process::ToWinAPI( serviceName );
		auto display = Process::ToWinAPI( displayName );
		auto path = Process::ToWinAPI( Process::GetExecutableFullPath() );
		auto acc = Process::ToWinAPI( account );
		auto pwd = Process::ToWinAPI( password );


		// Create a single buffer containing the dependencies separated by a null-char
		int charCount = 0;

		for( auto& d : dependencies )
		{
			auto curr = Process::ToWinAPI( d );
			charCount += lstrlen( curr.get() ) + 1; // Add one for terminating null-char
		}

		auto bytesNeeded = charCount * sizeof( CharType );
		bytesNeeded += sizeof( CharType ); // Add one for the double null-char
		auto deps = std::make_unique<CharType[]>( bytesNeeded );
		ZeroMemory( deps.get(), bytesNeeded );

		// Copy data into the array, separated by a null-char
		int offset = 0;
		for( auto& d : dependencies )
		{
			auto curr = Process::ToWinAPI( d );
			auto len = lstrlen( curr.get() );
			StringCchCopy( deps.get() + offset, bytesNeeded - offset, curr.get() );
			offset += len + 1; // One for null-char
		}

		AutoCloser<SC_HANDLE> sc(
			CreateService(
				manager.Get(),                  // SCManager database
				service.get(),					// Name of service 
				display.get(),					// Name to display 
				SERVICE_QUERY_STATUS,           // Desired access 
				SERVICE_WIN32_OWN_PROCESS,      // Service type 
				startType,						// Service start type 
				SERVICE_ERROR_NORMAL,           // Error control type 
				path.get(),						// Service's binary 
				nullptr,                        // No load ordering group 
				nullptr,                        // No tag identifier 
				deps.get(),						// Dependencies 
				acc.get(),						// Service running account 
				pwd.get()						// Password of the account 
			),
			myServiceCloser );


		result = sc.Get() != nullptr;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
bool Serviceize::Uninstall( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const
{
	bool result = false;

	AutoCloser<SC_HANDLE> manager( OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT ), myServiceCloser );
	if( manager.Get() != nullptr )
	{
		if( Stop( serviceName, maxWaitTime ) )
		{
			auto name = Process::ToWinAPI( serviceName );
			AutoCloser<SC_HANDLE> sc( OpenService( manager.Get(), name.get(), SERVICE_QUERY_STATUS | DELETE ), myServiceCloser );

			if( sc.Get() != nullptr )
			{

				if( DeleteService( sc.Get() ) )
				{
					result = true;
				}
			}
		}
	}


	return result;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
bool Serviceize::Stop( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const
{
	bool result = false;

	AutoCloser<SC_HANDLE> manager( OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT ), myServiceCloser );
	if( manager.Get() != nullptr )
	{

		auto name = Process::ToWinAPI( serviceName );

		AutoCloser<SC_HANDLE> sc( OpenService( manager.Get(), name.get(), SERVICE_STOP | SERVICE_QUERY_STATUS ), myServiceCloser );

		if( sc.Get() != nullptr )
		{

			SERVICE_STATUS status{};
			std::chrono::duration<uint64_t> maxWaitTime( 3s );
			auto start = std::chrono::steady_clock::now();

			// Is the service actually running?
			if( QueryServiceStatus( sc.Get(), &status )
				&& (status.dwCurrentState == SERVICE_STOPPED || status.dwCurrentState == SERVICE_STOP_PENDING) )
			{
				// Already stopped, or stopping.
				result = true;
			}
			else if( ControlService( sc.Get(), SERVICE_CONTROL_STOP, &status ) )
			{
				auto now = std::chrono::steady_clock::now();

				while( QueryServiceStatus( sc.Get(), &status )
					&& (status.dwCurrentState == SERVICE_STOP_PENDING || status.dwCheckPoint == SERVICE_RUNNING)
					&& (now - start) < maxWaitTime )
				{
					std::this_thread::sleep_for( 1s );
					now = std::chrono::steady_clock::now();
				}

				result = status.dwCurrentState == SERVICE_STOPPED;
			}

		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
bool Serviceize::Start( const std::string& serviceName, std::chrono::seconds maxWaitTime ) const
{
	bool result = false;

	AutoCloser<SC_HANDLE> manager( OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT ), myServiceCloser );
	if( manager.Get() != nullptr )
	{

		auto name = Process::ToWinAPI( serviceName );

		AutoCloser<SC_HANDLE> sc( OpenService( manager.Get(), name.get(), SERVICE_START | SERVICE_QUERY_STATUS ), myServiceCloser );

		if( sc.Get() != nullptr )
		{

			SERVICE_STATUS status{};
			std::chrono::duration<uint64_t> maxWaitTime( 3s );
			auto start = std::chrono::steady_clock::now();

			// Is the service already running?
			if( QueryServiceStatus( sc.Get(), &status )
				&& (status.dwCurrentState == SERVICE_START_PENDING || status.dwCurrentState == SERVICE_RUNNING) )
			{
				// Already running, or starting.
				result = true;
			}
			else				
			{
				// TODO: Allow passing arguments to the service start routine
				auto res = ::StartService( sc.Get(), 0, nullptr );
				result = res != 0;
			}

		}
	}

	return result;
}

}