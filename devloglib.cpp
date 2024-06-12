#include "stdafx.h"
#include "devloglib.h"

#define LOG_SLEEP_TIME			50

#define ACTIVE_BUSY				1
#define INACTIVE_BUSY			0

extern wchar_t app[ 260 ];

namespace wm_loglib
{
	static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
	static std::vector<std::wstring> queuemsg;

	pthread_t tid;
	int busy = ACTIVE_BUSY;

	void *executethread( void *arg )
	{
		int* busy = (int*)arg;
		while ( *busy ) {
			std::wstring msg(L"");

			pthread_mutex_lock( &log_mutex );

			if ( queuemsg.size() > 0 ) {
				msg = queuemsg.front();
				wprintf( L"%s\n", msg.c_str() );
				queuemsg.erase( queuemsg.begin(), queuemsg.begin() + 1 );
			}
			if ( msg.empty() )
				msleep( LOG_SLEEP_TIME );

			pthread_mutex_unlock( &log_mutex );
		}
		pthread_exit(0);
		return 0;
	}

	int create()
	{
		busy = ACTIVE_BUSY;

		_setmode(_fileno(stdout), _O_U16TEXT);
		_setmode(_fileno(stdin),  _O_U16TEXT);
		_setmode(_fileno(stderr), _O_U16TEXT);

		int rc = pthread_create( &tid, 0, executethread, &busy );
		if ( rc ) {
			__time64_t lt;
			struct tm u;

			wchar_t timeline[ 255 ] = { 0 };
			_time64( &lt );
			_localtime64_s( &u, &lt );

			wcsftime( timeline, 255, L"%d.%m.%Y %H:%M:%S", &u );
			wprintf( L"%s: FATAL\t logger thread failed", timeline );
		} else {
			msleep( LOG_SLEEP_TIME );
			wm_loglib::log( LOG_INFO, L"logger thread created" );
		}
		return ( rc > 0 ) ? 0xff : 0x00;
	}

	void destroy()
	{
		wm_loglib::log(LOG_DEBUG, L"[ %s ] logger thread exit", app);

		msleep( LOG_SLEEP_TIME * 30 );
		busy = INACTIVE_BUSY;

		pthread_join( tid, 0 );
	}

	void log( int _l, wchar_t* _fmt, ... )
	{
		pthread_mutex_lock( &log_mutex );

		__time64_t lt;
		struct tm u;

		_time64( &lt );
		_localtime64_s( &u, &lt );

		wchar_t timeline[ 255 ] = { 0 };
		wchar_t line[ 1024 ] = { 0 };

		wcsftime( timeline, 255, L"%d.%m.%Y %H:%M:%S", &u );

		wchar_t* wl = 0;

		if (_l == LOG_DEBUG) {
			wl = L"DEBUG";
		} else if (_l == LOG_WARNING){
			wl = L"WARNING";
		} else if (_l == LOG_ERROR) {
			wl = L"ERROR";
		} else if (_l == LOG_INFO) {
			wl = L"INFO";
		}

		wsprintf( line, L"%s %s\t", timeline, wl );

		std::wstring msg( line );

		va_list list;
		va_start( list, _fmt );

		_vsnwprintf( line, sizeof( line ) / sizeof( line[0] ), _fmt, list );

		va_end( list );

		msg += line;

		queuemsg.push_back( msg );

		pthread_mutex_unlock( &log_mutex );
	}
}