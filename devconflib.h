#pragma once

#ifndef __WM_DEVCONF_LIB__
#define __WM_DEVCONF_LIB__

#include <vector>

namespace wm_conflib
{
	int create( const wchar_t* filename = 0 );
	void destroy();

	std::string param_config_get( const char* nameparam, const char* defaultvalue );
	std::wstring param_config_get( const wchar_t* nameparam, const wchar_t* defaultvalue );

	void param_config_set( const char* nameparam, const char* newvalue );
	void param_config_set( const wchar_t* nameparam, const wchar_t* newvalue );

	int parse( wchar_t* buf );
	
	std::string utf8_wstomb( const wchar_t* s );							/* utf-8 wide string to multi byte */
	std::wstring utf8_mbtows( const char* s );								/* multi byte to utf-8 wide string */

	std::string wstomb( const wchar_t* s );
	std::wstring mbtows( const char* s );

	int utf8_wctomb (unsigned char *r, unsigned int wc, size_t n);			/* n == 0 is acceptable */
	int utf8_mbtowc (unsigned int *pwc, const unsigned char *s, size_t n);

	int config_save();

	template<class T> int length( const T* s )
	{
		int nl = 0;
		int ns = sizeof( s[0] );
		while( *s != 0 ) {
			++s;
			nl += ns;
		};
		return nl;
	};
}

#endif