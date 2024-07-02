#pragma once

#ifndef __WM_DEVUTILS_LIB__
#define __WM_DEVUTILS_LIB__

#if _WIN32 || _WIN64

#pragma warning( push )
#pragma warning( disable : 4005 )

#include <rpc.h>

#else

#include <sys/types.h>
#include <uuid/uuid.h>

#endif

#include <algorithm>
#include <string>

namespace wm_utils
{
	// wchar_t* rs_skip(wchar_t* wp, const wchar_t* wpattern);
	// wchar_t* rs_until(wchar_t* wp, std::wstring& word, const wchar_t* wpattern);
	wchar_t* ls_skip(wchar_t* wp, const wchar_t* wpattern);
	wchar_t* ls_until(wchar_t* wp, std::wstring& word, const wchar_t* wpattern);

	std::string uuid();
	std::wstring wuuid();
};

#endif
