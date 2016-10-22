#pragma once

#include <functional>

template< typename T>
class AutoCloser
{
public:
	AutoCloser( T t, std::function<void( T )> closer )
		: myT( t ), myCloser( closer )
	{
	}

	~AutoCloser()
	{
		myCloser( myT );
	}

	T Get() const { return myT; }

private:
	const T myT;
	std::function<void( T )> myCloser;
};