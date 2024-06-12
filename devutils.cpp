#include "stdafx.h"
#include "devutils.h"

extern wchar_t app[260];

namespace wm_utils {

	wchar_t* ls_skip(wchar_t* wp, const wchar_t* wpattern)
	{
		do {
			wchar_t ch = *wp;
			const wchar_t* pattern = wpattern;
			int f = 0;
			do {
				if (*pattern == ch) {
					f = 1;
					break;
				}
				++pattern;
			} while (*pattern != 0);
			if (f == 1) {
				++wp;
			}
			else break;
		} while (*wp != 0);
		return wp;
	}

	wchar_t* ls_until(wchar_t* wp, std::wstring& word, const wchar_t* wpattern)
	{
		do {
			wchar_t ch = *wp;
			const wchar_t* pattern = wpattern;
			int f = 0;
			do {
				if (*pattern == ch) {
					f = 1;
					break;
				}
				++pattern;
			} while (*pattern != 0);
			if (f == 0) {
				word += *wp;
				++wp;
			}
			else break;
		} while (*wp != 0);
		return wp;
	}

}