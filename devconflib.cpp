#include "stdafx.h"

#include "devconflib.h"
#include "devloglib.h"
#include "devutils.h"

extern wchar_t app[ 260 ];

namespace wm_conflib
{
	static wchar_t _filename[ 260 ];

	typedef struct confparam {
		wchar_t* p;
		wchar_t* a;
	} *ptrconfparam;

	static wchar_t* buf = 0;
	static std::vector<ptrconfparam> config;

	int create( const wchar_t* filename )
	{
		if ( filename ) wcscpy( _filename, filename );
		else wcscpy( _filename, L"service.conf" );
		FILE* f = _wfopen( _filename, L"ab+" );
		if ( f != 0 ) {
			fseek( f, 0, SEEK_END );
			long fs = ftell( f );
			if ( fs > 0 ) {
				fseek( f, 0, SEEK_SET );
				buf = (wchar_t*)::malloc( fs + sizeof(wchar_t) );
				if ( buf ) {
					memset( buf, 0, fs + sizeof(wchar_t) );
					size_t sz = fread( buf, sizeof(char), fs, f );
					if ( sz != fs ) {
						if ( buf ) ::free( buf );
						buf = 0;
						wm_loglib::log( LOG_INFO, L"could not read file %s", _filename );
						return -1;
					}
					fclose( f );
					((char*)(buf))[ fs + 0 ] = 0x00;
					((char*)(buf))[ fs + 1 ] = 0x00;
					int l = parse( buf );
					if ( l != 0 ) {
						wm_loglib::log( LOG_ERROR, L"configuration syntax error at line %d", l );
						return -1;
					}
				}
			}
		} else {
			wm_loglib::log( LOG_INFO, L"could not open file %s", _filename );
			return -1;
		}
		return 0;
	}

	int parse( wchar_t* buf ) 
	{
		std::wstring word;
		//////////////////////////////////////////
		// parsing
		//////////////////////////////////////////
		std::wstring param;
		std::wstring arg;
		int nl = 0;
		int nc = 1;
		wchar_t* wp = buf;
		do {
			wchar_t ch = *wp;
			wp = wm_utils::left_skip( wp, L"\t " );
			word = L"";
			wp = wm_utils::left_until( wp, word, L":\r\n" );
			if ( *wp == '\r' || *wp == '\n' ) {
				nl = nc;
				break;
			}
			param = word;
			wp = wm_utils::left_skip( wp, L":\t " );
			word = L"";
			wp = wm_utils::left_until( wp, word, L"\r\n" );
			arg = word;
			wp = wm_utils::left_skip( wp, L"\r\n" );
			param_config_set( param.c_str(), arg.c_str() );
			++nc;
		} while( *wp != 0 );
		return nl;
	}

	void destroy()
	{
		if ( buf ) ::free( buf );
		while ( config.size() > 0 ) {
			ptrconfparam ptr = config.back();
			if ( ptr->p ) ::free( ptr->p );
			if ( ptr->a ) ::free( ptr->a );
			if ( ptr ) ::free( ptr );
			config.pop_back();
		}
		wm_loglib::log( LOG_DEBUG, L"[ %s ] config cleanup", app );
	}

	std::string param_config_get( const char* nameparam, const char* defaultvalue )
	{
		std::string a1( nameparam );
		std::string a2( defaultvalue );

		std::wstring w1( a1.begin(), a1.end() );
		std::wstring w2( a2.begin(), a2.end() );

		std::wstring rr = param_config_get( w1.c_str(),  w2.c_str() );
		return std::string( rr.begin(), rr.end() );
	}

	std::wstring param_config_get( const wchar_t* nameparam, const wchar_t* defaultvalue )
	{
		for ( int i = 0; i < config.size(); i++ ) {
			ptrconfparam ptr = config[i];
			if ( wcscmp( ptr->p, nameparam ) == 0 )
				return std::wstring ( ptr->a );
		}
		return std::wstring ( defaultvalue );
	}

	void param_config_set( const char* nameparam, const char* newvalue )
	{
		std::string a1( nameparam );
		std::string a2( newvalue );

		std::wstring w1( a1.begin(), a1.end() );
		std::wstring w2( a2.begin(), a2.end() );

		param_config_set( w1.c_str(), w2.c_str() );
	}
	
	void param_config_set( const wchar_t* nameparam, const wchar_t* newvalue )
	{
		int rc = 0;
		for ( int i = 0; i < config.size(); i++ ) {
			ptrconfparam ptr = config[i];
			if ( wcscmp( ptr->p, nameparam ) == 0 ) {
				if ( ptr->a ) ::free( ptr->a );
				ptr->a = _wcsdup( newvalue );
				rc = 0xff;
			}
		}
		if ( rc == 0 ) {
			ptrconfparam ptr = (ptrconfparam)::malloc( sizeof(confparam) );
			if ( ptr ) {
				ptr->p = _wcsdup( nameparam );
				ptr->a = _wcsdup( newvalue );
				config.push_back( ptr );
			}
		}
	}

	std::string wstomb( const wchar_t* s )
	{
		std::wstring ws( s );
		return std::string( ws.begin(), ws.end() );
	}

	std::wstring mbtows( const char* s )
	{
		std::string ss( s );
		return std::wstring( ss.begin(), ss.end() );
	}

	std::string utf8_wstomb( const wchar_t* s )
	{
		std::string out = "";
		wchar_t* p = const_cast<wchar_t*>(s);
		while ( *p != 0 ) {
			unsigned char wc[4];
			int c = utf8_wctomb( wc, *p, 2 );
			if ( c > 0 )
				for ( int i = 0; i < c; i++ )
					out += wc[i];
			p = p + 1;
		}
		return out;
	}

	std::wstring utf8_mbtows( const char* s )
	{
		std::wstring out = L"";
		char* p = const_cast<char*>(s);
		while ( *p != 0 ) {
			unsigned int wc = 0;
			int c = utf8_mbtowc( (unsigned int*)&wc, reinterpret_cast<unsigned char*>(p), 2 );
			if ( c > 0 ) {
				out += (wchar_t)wc;
				p = p + c;
			}
		}
		return out;
	}

/*
	int utf16_mbtowc (unsigned int *pwc, const unsigned char *s, size_t n)
	{
		int count = 0;
		if (n >= 2) {
			unsigned int wc = s[0] + (s[1] << 8);
			if (wc >= 0xd800 && wc < 0xdc00) {
				if (n >= 4) {
					unsigned int wc2 = s[2] + (s[3] << 8);
					if (!(wc2 >= 0xdc00 && wc2 < 0xe000))
						goto ilseq;
					*pwc = 0x10000 + ((wc - 0xd800) << 10) + (wc2 - 0xdc00);
					return count+4;
				}
			} else if (wc >= 0xdc00 && wc < 0xe000) {
				goto ilseq;
			} else {
				*pwc = wc;
				return count+2;
			}
		}

		return -2 -2 * count;
	ilseq:
		return -1 -2 * count;
	}

	int utf16_wctomb ( unsigned char *r, unsigned int wc, size_t n)
	{
		if (!(wc >= 0xd800 && wc < 0xe000)) {
			if (wc < 0x10000) {
				if (n >= 2) {
					r[0] = (unsigned char) wc;
					r[1] = (unsigned char) (wc >> 8);
					return 2;
				} else
				return -2;
			} else if (wc < 0x110000) {
				if (n >= 4) {
					unsigned int wc1 = 0xd800 + ((wc - 0x10000) >> 10);
					unsigned int wc2 = 0xdc00 + ((wc - 0x10000) & 0x3ff);
					r[0] = (unsigned char) wc1;
					r[1] = (unsigned char) (wc1 >> 8);
					r[2] = (unsigned char) wc2;
					r[3] = (unsigned char) (wc2 >> 8);
					return 4;
				} else
				return -2;
			}
		}
		return -1;
	}
*/

	int utf8_mbtowc (unsigned int *pwc, const unsigned char *s, size_t n)
	{
		unsigned char c = s[0];

		if (c < 0x80) {
			*pwc = c;
			return 1;
		} else if (c < 0xc2) {
			return 0;
		} else if (c < 0xe0) {
			if (n < 2)
				return -1;
			if (!((s[1] ^ 0x80) < 0x40))
				return 0;
			*pwc = ((unsigned int) (c & 0x1f) << 6)
			   | (unsigned int) (s[1] ^ 0x80);
			return 2;
		} else if (c < 0xf0) {
			if (n < 3)
				return -1;
			if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
				&& (c >= 0xe1 || s[1] >= 0xa0)
				&& (c != 0xed || s[1] < 0xa0)))
			return 0;
			*pwc = ((unsigned int) (c & 0x0f) << 12)
			   | ((unsigned int) (s[1] ^ 0x80) << 6)
			   | (unsigned int) (s[2] ^ 0x80);
			return 3;
		} else if (c < 0xf8 && sizeof(unsigned int)*8 >= 32) {
			if (n < 4)
				return -1;
			if (!((s[1] ^ 0x80) < 0x40 && (s[2] ^ 0x80) < 0x40
				&& (s[3] ^ 0x80) < 0x40
				&& (c >= 0xf1 || s[1] >= 0x90)
				&& (c < 0xf4 || (c == 0xf4 && s[1] < 0x90))))
			return 0;
			*pwc = ((unsigned int) (c & 0x07) << 18)
				| ((unsigned int) (s[1] ^ 0x80) << 12)
				| ((unsigned int) (s[2] ^ 0x80) << 6)
				| (unsigned int) (s[3] ^ 0x80);
			return 4;
		} 
		return 0;
	}

	int utf8_wctomb (unsigned char *r, unsigned int wc, size_t n) /* n == 0 is acceptable */
	{
		int count;
		if (wc < 0x80)
			count = 1;
		else if (wc < 0x800)
			count = 2;
		else if (wc < 0x10000) {
		if (wc < 0xd800 || wc >= 0xe000)
			count = 3;
		else
			return 0;
		} else if (wc < 0x110000)
			count = 4;
		else
			return 0;
		if (n < count)
			return -1;
		switch (count) { /* note: code falls through cases! */
		case 4: r[3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
		case 3: r[2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
		case 2: r[1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
		case 1: r[0] = wc;
	  }
	  return count;
	}

	int config_save()
	{
		FILE* f = _wfopen( _filename, L"wb+" );
		if ( f != 0 ) {
			for ( size_t i = 0; i < config.size(); i++ ) {
				ptrconfparam ptr = config[i];
				if ( ptr ) {
					fwrite( ptr->p, wcslen( ptr->p ) * sizeof(wchar_t), 1, f );
					wchar_t *d = 0;
					d = L": ";
					fwrite( d, wcslen( d ) * sizeof(wchar_t), 1, f );
					fwrite( ptr->a, wcslen( ptr->a ) * sizeof(wchar_t), 1, f );
					d = L"\r\n";
					fwrite( d, wcslen( d ) * sizeof(wchar_t), 1, f );
				}
			}
			fclose( f );
		} else {
			wm_loglib::log( LOG_INFO, L"could not open file %s", _filename );
			return -1;		
		}
		return 0;
	}
}