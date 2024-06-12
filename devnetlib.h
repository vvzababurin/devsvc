#pragma once

#ifndef __WM_DEVNET_LIB__
#define __WM_DEVNET_LIB__

#if _WIN32 || _WIN64

#pragma warning( push )
#pragma warning( disable : 4005 )

#define FD_SETSIZE				2048

#include <rpc.h>
#include <winsock.h>

#define SD_RECEIVE				0x00
#define SD_SEND					0x01
#define SD_BOTH					0x02

#define closesock(s)				{ shutdown( s, SD_BOTH ); closesocket(s); }
#define getsocketerrno				WSAGetLastError()
#define setsocketerrno(e)			WSASetLastError(e)
#define isvalidsock(s)				( (s) > 0 ) 

#else

#define FD_SETSIZE				2048

#include <sys/types.h>
#include <sys/socket.h>
#include <uuid/uuid.h>

#define WSAEWOULDBLOCK				EWOULDBLOCK

#define closesock(s)				{ shutdown( s, SHUT_RDWR ); close(s); }
#define getsocketerrno				errno
#define setsocketerrno(e)			seterrno(e)
#define isvalidsock(s)				( (s) > 0 ) 

#endif

#include <algorithm>
#include <string>

#define BUF_SIZ					4096

namespace wm_netlib
{
	static bool running = true;

	int create();
	void destroy();

	int setnonblockopt(int s, unsigned long _block);

	int readn(int s, char* p, int l);
	int sendn(int s, const char* p, int l);

	int readn(int s, char** pp, int, int, int);

	std::string uuid();
	std::wstring wuuid();

	int create_tcpcli( const char*, int );
	int create_tcpsrv( const char*, int, int, int(*)(int,char**), int(*)(int), int(*)(int, const char*, int) );

	int wait( int, int, int );
}

#if _WIN32 || _WIN64
#pragma warning( pop )
#endif

#endif