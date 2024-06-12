#pragma once

#ifndef __WM_DEVLOG_LIB__
#define __WM_DEVLOG_LIB__

#define LOG_INFO				0x00
#define LOG_ERROR				0x01
#define LOG_WARNING				0x02
#define LOG_DEBUG				0x03

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>

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

namespace wm_loglib
{
	int create();
	void destroy();

	void log(int l, wchar_t* _fmt, ...);
	void *executethread(void *);
}

#endif