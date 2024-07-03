#include "stdafx.h"

#include "devprotolib.h"
#include "devnetlib.h"
#include "devloglib.h"
#include "devtasklib.h"

#include <signal.h>

#if _WIN32 || _WIN64
#pragma warning( push )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4005 )
#endif

extern wchar_t app[ 260 ];

namespace wm_protolib
{
	int create()
	{
		return 0;
	}

	void destroy()
	{
	
	}
}