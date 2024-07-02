#include "stdafx.h"

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

namespace wm_netlib
{
	static pthread_mutex_t fdset_mutex = PTHREAD_MUTEX_INITIALIZER;

	typedef struct params {
		int (*read_cb)(int,char**);
		fd_set* readfds;
		int cs;
	} *ptrparams;

	static inline void signalhandler( int sig ) {
		wchar_t* signal = 0;

		switch ( sig )
		{
		case SIGINT:
			signal = L"SIGINT";
			break;
		case SIGBREAK:
			signal = L"SIGBREAK";
			break;
		case SIGABRT:
			signal = L"SIGABRT";
			break;
		case SIGTERM:
			signal = L"SIGTERM";
			break;
		}

		wm_loglib::log( LOG_INFO, L"signal %s received, exiting", signal);
		running = false;
	}

	static inline void readhandler( void* arg ) 
	{
		//////////////////////////////////////////////////
		// read incoming connection
		//////////////////////////////////////////////////
		ptrparams ptr = (ptrparams)arg;
		if ( ptr > 0 ) 
		{
			if ( ptr->read_cb > 0 ) 
			{
				char* buf = 0;
				int cb = ptr->read_cb( ptr->cs, &buf );
				if ( cb > 0 ) {
					pthread_mutex_lock( &fdset_mutex );
					FD_SET( ptr->cs, ptr->readfds );
					pthread_mutex_unlock( &fdset_mutex );
					wm_loglib::log( LOG_INFO, L"recv packet size %d", cb );
				} else {
					closesock( ptr->cs );
					wm_loglib::log( LOG_INFO, L"close socket %d", ptr->cs );
				}
				if ( buf > 0 ) ::free( buf );
			}
			if ( ptr > 0 ) ::free( ptr );
		}
	}

	int create()
	{
#if _WIN32 || _WIN64
		WSAData wd;
		int rc = WSAStartup( MAKEWORD( 2, 2 ), &wd );
#else
		int rc = 0;
#endif
		wm_loglib::log( LOG_DEBUG, L"network startup" );
		return rc;
	}

	void destroy()
	{
#if _WIN32 || _WIN64
		WSACleanup();
#endif
		wm_loglib::log( LOG_DEBUG, L"[ %s ] network cleanup", app );
	}

	int setnonblockopt( int s, unsigned long _nonblock = 1 )
	{
#if _WIN32 || _WIN64
		const unsigned long on = _nonblock ? 1 : 0;
		return ioctlsocket( s, FIONBIO, (unsigned long *)&on );
#elif
		int flags = 0;
		if ( ( flags = fcntl( s, F_GETFL, 0 ) ) < 0 )
			return -1;
		if ( _nonblock > 0 )
			if ( fcntl( s, F_SETFL, flags | O_NONBLOCK) < 0 )
				return -1;
		else
			if ( fcntl( s, F_SETFL, flags & O_NONBLOCK) < 0 )
				return -1;
		return 0;
#endif
	}
	
	int readn( int s, char*p, int l )
	{
		int c = 0;
		do {
			int r = recv( s, p, l - c, 0 );
			if ( r == 0 ) return 0; // gracefully closed
			if ( r < 0 ) {
				if (getsocketerrno == WSAEWOULDBLOCK)
					if ( c > 0 ) return c;
				return -1;
			}
			c += r;
			p += r;
		} while ( c != l );
		return c; // all data is been read
	}

	int sendn( int s, const char*p, int l )
	{
		int c = l;
		do {
			int r = send( s, p, c, 0 );
			if ( r < 0 ) {
				if (getsocketerrno == WSAEWOULDBLOCK) continue;
				return -1;
			}
			c -= r;
			p += r;
		} while ( c != 0 );
		return l;
	}

	int wait( int s, int sec, int usec )
	{
		struct timeval tv;
			
		tv.tv_sec = sec;
		tv.tv_usec = usec;

		fd_set rfds;
		::memset( &rfds, 0, sizeof( rfds ) );

		FD_SET( s, &rfds );

		int nfds = s;

		int fdn = select( nfds + 1, &rfds, 0, 0, &tv );
		if ( fdn == 0 ) return 0;
		if ( fdn > 0 ) {
			if ( FD_ISSET( s, &rfds ) )
				return 1;
		}

		return -1;
	}

	int readn(int s, char** pp, int maxsize, int sec, int usec )
	{
		int rr = wm_netlib::wait( s, sec, usec );
		if ( rr ) {
			char* p = 0;
			int bs = 0;
			do {		
				p = ( p == 0 ) ? (char*)::malloc( bs + BUF_SIZ ) : (char*)::realloc( p, bs + BUF_SIZ );
				char* buf = p + sizeof( p[0] ) * bs;
				memset(buf, 0, BUF_SIZ);
				int cb = wm_netlib::readn( s, buf, BUF_SIZ );
				if ( cb == BUF_SIZ ) {
					bs += cb;
					if ( bs > maxsize ) {
						bs = maxsize;
						wm_loglib::log( LOG_DEBUG, L"max packet size exceeded ( len = %d )", bs );
						break;
					}
					continue;
				}
				if ( cb > 0 ) bs += cb;
				break;
			} while ( true );
			wm_loglib::log( LOG_DEBUG, L"packet was readed ( len = %d )", bs );
			*pp = p;
			return bs;
		}
		return rr; /*  0 or -1  */
	}

/*
	int create_udpsrv()
	{
		struct sockaddr_in peer;
		int s;

		peer.sin_family = AF_INET;
		peer.sin_port = htons(port);
		peer.sin_addr.s_addr = inet_addr(addr);

		s = (int)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (!isvalidsock(s)) {
			wm_loglib::log(LOG_ERROR, L"%s %d", L"socket() failed: ", getsocketerrno);
			goto out;
		}

		return 0;
	}
*/

	int create_tcpcli( const char* addr, int port )
	{
		struct sockaddr_in peer;
		int s;

		peer.sin_family = AF_INET;
		peer.sin_port = htons( port );
		peer.sin_addr.s_addr = inet_addr( addr );

		s = (int)socket ( AF_INET, SOCK_STREAM, 0 );
		if (!isvalidsock(s)) {
			wm_loglib::log(LOG_ERROR, L"%s %d", L"socket() failed: ", getsocketerrno );
			goto out;
		}

		if (connect ( s, ( struct sockaddr*)&peer, sizeof ( peer ) ) < 0) {
			wm_loglib::log(LOG_ERROR, L"%s %d", L"connect() failed: ", getsocketerrno );
			goto out;
		}

		if (setnonblockopt( s, 1 ) < 0) {
			wm_loglib::log(LOG_ERROR, L"%s %d", L"setnonblockopt() failed: ", getsocketerrno );
			goto out;
		}

		return s;

	out:
		closesock( s );
		return s;
	}

	int create_tcpsrv( const char* addr, int port, int listeners, int(*read_cb)(int,char**) = 0, int(*write_cb)(int) = 0, int(*accept_cb)(int, const char*, int) = 0 )
	{
		struct sockaddr_in local;
		const int on = 1;

		signal(SIGINT, signalhandler);
#ifdef SIGBREAK
		signal(SIGBREAK, signalhandler);
#endif
		signal(SIGABRT, signalhandler);
		signal(SIGTERM, signalhandler);

		int s = ( int )socket ( AF_INET, SOCK_STREAM, 0 );
		if ( !isvalidsock ( s ) ) {
			wm_loglib::log(LOG_ERROR, L"%s %d", L"socket() failed", getsocketerrno );
			goto out;
		}

		local.sin_family = AF_INET;
		local.sin_port = htons( port );

		if ( strcmp( addr, "INADDR_ANY" ) == 0 )
			local.sin_addr.s_addr = ntohl(INADDR_ANY);
		else
			local.sin_addr.s_addr = inet_addr(addr);

		if ( setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) ) {
			wm_loglib::log(LOG_ERROR, L"setsockopt() failed %d",  getsocketerrno );
			goto out;
		}

		if ( bind ( s, ( struct sockaddr* )&local, sizeof ( local ) ) ) {
			wm_loglib::log(LOG_ERROR, L"bind() failed %d", getsocketerrno );
			goto out;
		}

		if ( listen ( s, listeners ) ) {
			wm_loglib::log(LOG_ERROR, L"listen() failed %d", getsocketerrno );
			goto out;
		}

		if ( setnonblockopt( s, true ) < 0 ) {
			wm_loglib::log(LOG_ERROR, L"setnonblockopt() failed %d", getsocketerrno );
			goto out;
		}

		if ( isvalidsock( s ) ) 
		{
			fd_set readfds;
			::memset( &readfds, 0, sizeof( readfds ) );

			FD_SET( s, &readfds );

			int nfds = s;

			while ( running )
			{
				struct timeval tv;
			
				tv.tv_sec = 0;
				tv.tv_usec = 1000; // 100000

				fd_set rfds;
				fd_set wfds;
				fd_set efds;

				::memset( &rfds, 0, sizeof( rfds ) );
				::memset( &wfds, 0, sizeof( wfds ) );
				::memset( &efds, 0, sizeof( efds ) );

				pthread_mutex_lock( &fdset_mutex );

				for ( u_int i = 0; i < readfds.fd_count; i++ )
					nfds = ( nfds > readfds.fd_array[i] ) ? nfds : readfds.fd_array[i];

				rfds = readfds;			// read, accept
				wfds = readfds;			// connect
				efds = readfds;			// exceptions

				pthread_mutex_unlock( &fdset_mutex );

				int fdn = select( nfds + 1, &rfds, &wfds, &efds, &tv );
				if ( fdn == 0 ) continue;
				if ( fdn > 0 ) 
				{
					for ( u_int i = 0; i < wfds.fd_count; i++ ) 
					{
						if ( write_cb > 0 ) {
							write_cb( wfds.fd_array[i] );
						}
					}
					for ( u_int i = 0; i < efds.fd_count; i++ ) 
					{
						int fd = efds.fd_array[i];
						pthread_mutex_lock( &fdset_mutex );
						FD_CLR( fd, &readfds );
						pthread_mutex_unlock( &fdset_mutex );
						closesock( fd );
						wm_loglib::log(LOG_ERROR, L"close socket descriptor %d for a %d exception", fd, getsocketerrno );
					}
					for ( u_int i = 0; i < rfds.fd_count; i++ ) 
					{
						if ( rfds.fd_array[i] == s ) 
						{
							if ( FD_ISSET( rfds.fd_array[i], &rfds ) ) 
							{
								//////////////////////////////////////////////////
								// accept incoming connection
								//////////////////////////////////////////////////

								struct sockaddr_in remote;
								int addrlen = sizeof(remote);

								int cs = accept( s, (struct sockaddr*)&remote, &addrlen );
								if ( isvalidsock( cs ) ) 
								{
									if (wm_netlib::setnonblockopt( cs, true ) < 0 )
									{
										wm_loglib::log( LOG_ERROR, L"setnonblockopt() failed %d", getsocketerrno );
										continue;
									}
									if ( accept_cb > 0 ) 
									{
										int rc = accept_cb( cs, inet_ntoa( remote.sin_addr ), ntohs( remote.sin_port ) );
										if ( rc > 0 ) {
											//pthread_mutex_lock( &fdset_mutex );
											//FD_SET( cs, &readfds );
											//pthread_mutex_unlock( &fdset_mutex );
											wm_loglib::log(LOG_INFO, L"accept incoming connection from: %S:%d", inet_ntoa( remote.sin_addr ), ntohs( remote.sin_port ) );
										} else {
											wm_loglib::log(LOG_INFO, L"reject incoming connection from: %S:%d", inet_ntoa( remote.sin_addr ), ntohs( remote.sin_port ) );
											closesock( cs );
											continue;
										}
									} 
									else
									{
										//pthread_mutex_lock( &fdset_mutex );
										//FD_SET( cs, &readfds );
										//pthread_mutex_unlock( &fdset_mutex );
										wm_loglib::log(LOG_INFO, L"accept incoming connection as a default from: %S:%d", inet_ntoa( remote.sin_addr ), ntohs( remote.sin_port ) );
									}

									//pthread_mutex_lock( &fdset_mutex );
									//FD_CLR( cs, &readfds );
									//pthread_mutex_unlock( &fdset_mutex );

									ptrparams ptr = (ptrparams)::malloc( sizeof(params) );
									if ( ptr && read_cb ) {
										ptr->read_cb = read_cb;
										ptr->cs = cs;
										ptr->readfds = &readfds;

										wm_tasklib::execute( readhandler, (void*)ptr );
									}
								}
							}
							continue;
						}
						else if ( FD_ISSET( rfds.fd_array[i], &rfds ) ) 
						{
							pthread_mutex_lock( &fdset_mutex );
							FD_CLR( rfds.fd_array[i], &readfds );
							pthread_mutex_unlock( &fdset_mutex );

							ptrparams ptr = (ptrparams)::malloc( sizeof(params) );
							if ( ptr && read_cb ) {
								ptr->read_cb = read_cb;
								ptr->cs = rfds.fd_array[i];
								ptr->readfds = &readfds;

								wm_tasklib::execute( readhandler, (void*)ptr );
							}
						}
					}
				}
			}
		}

	out:
		closesock( s );
		return 0;
	}

}

#if _WIN32 || _WIN64
#pragma warning( pop ) 
#endif
