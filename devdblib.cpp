#include "stdafx.h"

#include "devdblib.h"
#include "devtasklib.h"
#include "devloglib.h"

#pragma comment(lib, "libmysql.lib")

extern wchar_t app[ 260 ];

/*
typename struct db_modules_t
{
    char name[128];
    int (*m_create)();
    int (*m_destroy)();
};

int a()
{
    return 0;
}

db_modules_t modules[] = {
    { "mysql", a, a }
};

*/

namespace wm_dblib
{
    static MYSQL* db_connect = 0;

    int create( const std::string & _db_sqlhost, const std::string & _db_sqluser, const std::string & _db_sqlpasswd, const std::string & _db_sqldb )
    {
        db_sqlhost = _db_sqlhost;
	    db_sqluser = _db_sqluser;
	    db_sqlpasswd = _db_sqlpasswd;
	    db_sqldb = _db_sqldb;

	    db_connect = mysql_init( 0 );
	    if( db_connect == NULL ) {
            wm_loglib::log( LOG_ERROR, L"failed create mysql descriptor" );
		    return db_failed;
	    }

        return db_success;
    }

    void destroy()
    {
        // mysql_thread_end();
    }

    int connectdb( MYSQL* connect, const std::string &host, const std::string &db, const std::string &user, const std::string &passwd )
    {
	    MYSQL* conn = mysql_real_connect( connect, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 0, 0, 0 );
	    if( conn == NULL ) {
            wm_loglib::log( LOG_ERROR, L"failed connect to mysql" );
		    return db_failed;
	    }
        return db_success;           
    }

    static e_status update( const db_table& dbtable, const db_field& dbfield, const db_value& dbvalue )
    {
        return db_failed;
    }

}                  