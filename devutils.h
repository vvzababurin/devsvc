#pragma once

#ifndef __WM_DEVUTILS_LIB__
#define __WM_DEVUTILS_LIB__

namespace wm_utils
{
	// wchar_t* rs_skip(wchar_t* wp, const wchar_t* wpattern);
	// wchar_t* rs_until(wchar_t* wp, std::wstring& word, const wchar_t* wpattern);
	wchar_t* ls_skip(wchar_t* wp, const wchar_t* wpattern);
	wchar_t* ls_until(wchar_t* wp, std::wstring& word, const wchar_t* wpattern);
};

#endif
