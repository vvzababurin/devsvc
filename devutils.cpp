#include "stdafx.h"
#include "devutils.h"

extern wchar_t app[260];

#if _WIN32 || _WIN64
#pragma comment(lib, "rpcrt4.lib")
#endif

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

	std::string uuid()
	{
#if _WIN32 || _WIN64
		UUID uuid;
		UuidCreate( &uuid );

		unsigned char * str;
		UuidToStringA( &uuid, &str );

		std::string s( (char*)str );

		RpcStringFreeA ( &str );
#else
		uuid_t uuid;
		uuid_generate_random ( uuid );
		char str[37];
		uuid_unparse ( uuid, s );

		std::string s( ( char* ) str );
#endif
		std::transform( s.begin(), s.end(), s.begin(), [](char c){ return ::tolower(c); } );
		return s;
	}

	std::wstring wuuid()
	{
		std::string guid = uuid();
		return std::wstring( guid.begin(), guid.end() );
	}

}