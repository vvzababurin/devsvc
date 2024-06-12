#pragma once

#ifndef __WM_DEVTASK_LIB__
#define __WM_DEVTASK_LIB__

#if _WIN32 || _WIN64
#pragma warning( push )
#pragma warning( disable : 4005 )
#include <windef.h>
#include <winbase.h>
#define msleep(ms)		Sleep(ms)
#else
#include <unistd.h>
#define msleep(ms)		usleep(ms*1000)
#endif

#include <pthread.h>
#include <vector>

namespace wm_tasklib
{
	int create();
	void destroy();

	void execute( void(*func)(void*), void* arg );
	void *executethread(void *);
}

#if _WIN32 || _WIN64
#pragma warning( pop )
#endif

#endif