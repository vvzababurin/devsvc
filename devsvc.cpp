// devsvc.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include "devtasklib.h"
#include "devnetlib.h"
#include "devprotolib.h"
#include "devloglib.h"
#include "devconflib.h"
#include "devutils.h"
#include "devdblib.h"

#include "curve25519.h"
#include "base64.h"
#include "pthread.h"
#include "jsmn.h"

#if _WIN32 || _WIN64
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "rpcrt4.lib")

#ifndef _DEBUG
#pragma comment(lib, "libcurve25519.lib")
#pragma comment(lib, "libpthread.lib")
#else
#pragma comment(lib, "libcurved25519.lib")
#pragma comment(lib, "libpthreadd.lib")
#endif

//////////////////////////////////////////////////////////////////
// #pragma comment(lib, "libssl.lib")
// #pragma comment(lib, "libcrypto.lib")
//////////////////////////////////////////////////////////////////

#endif

static std::string privatekey = "";
static std::string publickey = "";

wchar_t app[ 260 ];

inline int makepacket( unsigned int packettype, std::string& data )
{
	std::stringstream ss;

	ss << std::string("{ ");
	ss << std::string("\"type\":") << packettype << std::string(",");
	ss << std::string("\"data\":\"") << data << std::string("\"");
	ss << std::string(" }");
}

inline int body_parsing( int pid, std::string& base64data )
{
    return 1;
    /*
		std::wstring wlogin = L"";
		std::wstring wpasswd = L"";
		std::wstring f_name = L"";
		std::wstring m_name = L"";
		std::wstring l_name = L"";
			std::wstring wguid = L"";
			std::wstring wphone = L"";
			std::wstring wemail = L"";
			std::string pubkey = "";

			int itype = -1;

			json_val* jsdataroot = json::parse( (wchar_t*)buf );
			if ( jsdataroot ) {
				json_val* jstype = jsdataroot->child(L"ntype");
				if ( !jstype ) {
					goto out;
				}
				json_val* jslogin = jsdataroot->child(L"login");
				if ( !jslogin ) {
					goto out;
				}
				json_val* jspasswd = jsdataroot->child(L"passwd");
				if ( !jspasswd ) {
					goto out;
				}
				json_val* jsfname = jsdataroot->child(L"f_name");
				if ( !jsfname ) {
					goto out;
				}
				json_val* jsmname = jsdataroot->child(L"m_name");
				if ( !jsmname ) {
					goto out;
				}
				json_val* jslname = jsdataroot->child(L"l_name");
				if ( !jslname ) {
					goto out;
				}
				json_val* jsphone = jsdataroot->child(L"phone");
				if ( !jsphone ) {
					goto out;
				}
				json_val* jsemail = jsdataroot->child(L"email");
				if ( !jsemail ) {
					goto out;
				}
				json_val* jsguid = jsdataroot->child(L"guid");
				if ( !jsguid ) {
					goto out;
				}
				itype = jstype->tointegernumber();
				if ( itype == 0x7f  ) {
					json_val* jspubkey = jsdataroot->child(L"pubkey");
					if ( !jspubkey ) {
						goto out;
					}
					pubkey = jspubkey->tochar_str();
				}

				wlogin = jslogin->towchar_str();
				wpasswd = jspasswd->towchar_str();
				f_name = jsfname->towchar_str();
				m_name = jsmname->towchar_str();
				l_name = jslname->towchar_str();
				wphone = jsphone->towchar_str();
				wemail = jsemail->towchar_str();
				wguid = jsguid->towchar_str();
				
				loglib::log( LOG_INFO, L"login: %s; password: %s", wlogin.c_str(), wpasswd.c_str() );
				loglib::log( LOG_INFO, L"%s %s %s", f_name.c_str(), m_name.c_str(), l_name.c_str() );

				if ( curve25519_verifybase64( (char*)signature.c_str(), pubkey.c_str(), (unsigned char*)base64data.c_str(), (unsigned long)base64data.size() ) == 0 ) {
					if ( itype == 0x7f ) {
						loglib::log( LOG_INFO, L"verify::success [ itype: 0x%02hhx ( \"hello message\" && key exchange ) ] ", itype );
					}
				}
out:
				delete jsdataroot;
                */
}

inline int parsing_data( int bs, char** pp )
{
    char *p = *pp;
    int retval = 0;



/*
  	json_val* jsroot = json::parse( (wchar_t*)p );
	if ( jsroot ) {
        json_val* jsid = jsroot->child(L"id");
        if ( !jsid ) {
			wm_loglib::log( LOG_ERROR, L"packet identifier field is missing" );
            delete jsroot;
            return -1;
        }
        if ( !jsid->isnumber() ) {
			wm_loglib::log( LOG_ERROR, L"packet identifier field is not a number" );
            delete jsroot;
            return -1;
        }
        int pid = jsid->tointegernumber();
        //////////////////////////////////////////////////////////
        //  0xffff ( 65535 ) - hello message
        //  0xff00 ( 65280 ) - authorization message
        //  0xff01 ( 65281 ) - authentication message
        //////////////////////////////////////////////////////////
		json_val* jssignature = jsroot->child(L"signature");
        if ( !jssignature ) {
            if ( pid != 0xffff ) {
				wm_loglib::log( LOG_ERROR, L"packet signature field is missing" );
                delete jsroot;
                return -1;
            }
			wm_loglib::log( LOG_INFO, L"packet signature is missing" );
        }
        std::string signature = "";
        if ( pid != 0xffff ) {
            signature = jssignature->tochar_str();
            if ( signature.empty() ) {
				wm_loglib::log( LOG_ERROR, L"packet signature field is missing" );
                delete jsroot;
                return -1;                        
            }
        }
        json_val* jsbase64data = jsroot->child(L"base64data");
        if ( !jsbase64data ) {
			wm_loglib::log( LOG_ERROR, L"packet base64data field is missing" );
            delete jsroot;
            return -1;            
        }
        std::string base64data = jsbase64data->tochar_str();
        if ( base64data.empty() ) {
			wm_loglib::log( LOG_ERROR, L"packet base64data field is missing" );
            delete jsroot;
            return -1;                        
        }
        json_val* jspubkey = jsroot->child(L"pubkey");
        if ( !jspubkey ) {
			wm_loglib::log( LOG_ERROR, L"packet pubkey field is missing" );
            delete jsroot;
            return -1;            
        }
        std::string pubkey = jspubkey->tochar_str();
        if ( pubkey.empty() ) {
			wm_loglib::log( LOG_ERROR, L"packet pubkey field is missing" );
            delete jsroot;
            return -1;                        
        }
        if ( pid != 0xffff ) {
            if ( curve25519_verifybase64( (char*)signature.c_str(), pubkey.c_str(), (unsigned char*)base64data.c_str(), (unsigned long)base64data.size() ) != 0 ) {
				wm_loglib::log( LOG_INFO, L"verify::failed" );
		        ////////////////////////////////////////////////
		        // TODO: сообщение службе безопастности
		        ////////////////////////////////////////////////
                delete jsroot;
                return -1;            
            } else {
				wm_loglib::log( LOG_INFO, L"verify::success" );
                retval = body_parsing( pid, base64data );
            }
        } else {
			wm_loglib::log( LOG_INFO, L"verify::unneeded" );
            retval = body_parsing( 0xffff, base64data );        
        }
		delete jsroot;
    }
	*/
    return retval;
}

inline int read_callback(int s, char** pp)
{
	char* p = 0;
	int bs = 0;

	do {		
		p = ( p == 0 ) ? (char*)::malloc( bs + BUF_SIZ ) : (char*)::realloc( p, bs + BUF_SIZ );
		char* buf = p + sizeof( p[0] ) * bs;
		memset(buf, 0, BUF_SIZ);
		int cb = wm_netlib::readn( s, buf, BUF_SIZ );
		if ( cb == BUF_SIZ ) {
			bs += cb;
			continue;
		}
		if ( cb > 0 ) bs += cb;
		////////////////////////////////////////////////
		// TODO: максимальный размер пакета DDoS
		////////////////////////////////////////////////
		break;
	} while ( true );
	*pp = p;

    int retval = bs;

    if ( bs > 0 )
        retval = parsing_data( bs, pp );
    
    ////////////////////////////////////////////////////////
    // ( retval <= 0 ) - close socket
    ////////////////////////////////////////////////////////

	return retval;
}

inline int write_callback(int s)
{
	return 0;
}

inline int accept_callback(int s, const char* addr, int port)
{
	wm_loglib::log( LOG_DEBUG, L"accept callback" );
	return 1;
}

int main(int argc, char* argv[])
{
    std::string mysqlhost = "";
	std::string mysqluser = "";
	std::string mysqlpasswd = "";
	std::string mysqldb = "";

	wchar_t config[ 260 ];

	char* appmodname = strrchr( argv[0], '/' );

	if ( appmodname == 0 ) appmodname = strrchr( argv[0], '\\' );
	appmodname = appmodname + sizeof(char);
	mbstowcs( app, appmodname, 260 );

	wcscpy( config, L"wm-");
	wcscat( config, app );
	wchar_t *wappmodname = wcsrchr( config, '.' );

	if ( wappmodname ) *wappmodname = 0;
	wcscat( config, L".config" );

	if ( wm_loglib::create() != 0 ) {
		wprintf( L"FATAL ERROR: failed initialize loglib" );
		return -1;
	}

	if ( wm_conflib::create( config ) != 0 ) {
		wm_loglib::log( LOG_ERROR, L"failed initialize conflib" );
		goto conflib_exit;
	}

	privatekey = wm_conflib::param_config_get( "privatekey", "" );
	publickey = wm_conflib::param_config_get( "publickey", "" );

	if ( privatekey.empty() || publickey.empty() ) 
	{
		char* privkey = 0;
		char* pubkey = 0;

		privatekey = "";
		publickey = "";

		curve25519_privkeygenbase64( &privkey );
		curve25519_keygenbase64( &pubkey, privkey );

		privatekey = privkey;
		publickey = pubkey;

		wm_conflib::param_config_set( "privatekey", privatekey.c_str() );
		wm_conflib::param_config_set( "publickey", publickey.c_str() );

		wm_conflib::config_save();
	}

    mysqlhost = wm_conflib::param_config_get( "mysqlhost", "localhost" );
	mysqluser = wm_conflib::param_config_get( "mysqluser", "" );
	mysqlpasswd = wm_conflib::param_config_get( "mysqlpasswd", "" );
	mysqldb = wm_conflib::param_config_get( "mysqldb", "" );

	if ( wm_dblib::create(mysqlhost, mysqluser, mysqlpasswd, mysqldb) != 0 ) {
		wm_loglib::log( LOG_ERROR, L"failed initialize dblib" );
		goto dblib_exit;
	}

	if ( wm_netlib::create() != 0 ) {
		wm_loglib::log( LOG_ERROR, L"failed initialize netlib" );
		goto netlib_exit;
	}

	if ( wm_protolib::create() != 0 ) {
		wm_loglib::log( LOG_ERROR, L"failed initialize protolib" );
		goto protolib_exit;
	}

	if (wm_tasklib::create() != 0 ) {
		wm_loglib::log( LOG_ERROR, L"failed initialize tasklib" );
		goto tasklib_exit;
	}

	goto do_main;

tasklib_exit:
	wm_tasklib::destroy();

protolib_exit:
	wm_protolib::destroy();

netlib_exit:
	wm_netlib::destroy();

dblib_exit:
	wm_dblib::destroy();

conflib_exit:
	wm_conflib::destroy();

loglib_exit:
	wm_loglib::destroy();

	return -1;

do_main:

	std::wstring guid = wm_utils::wuuid();

	do {
		int rr = wm_netlib::create_tcpsrv( "INADDR_ANY", 8500, FD_SETSIZE, read_callback, write_callback, accept_callback );
		if ( rr == 0 ) {
			wm_loglib::log( LOG_INFO, L"[ %s ] shutdown service",  app );
			break;
		} else {
			wm_loglib::log( LOG_INFO, L"[ %s ] restart service",  app );
		}
	} while ( true );

//common_exit:

	wm_tasklib::destroy();
	wm_protolib::destroy();
	wm_netlib::destroy();
	wm_dblib::destroy();
	wm_conflib::destroy();
	wm_loglib::destroy();

	return 0;
}

