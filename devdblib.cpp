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
    static MYSQL* t_connect = 0;

    int create( const std::string & _dbsqlhost, const std::string & _dbsqluser, const std::string & _dbsqlpasswd, const std::string & _dbsqldb )
    {
        dbsqlhost = _dbsqlhost;
	    dbsqluser = _dbsqluser;
	    dbsqlpasswd = _dbsqlpasswd;
	    dbsqldb = _dbsqldb;

	    t_connect = mysql_init( 0 );
	    if( t_connect == NULL ) {
            wm_loglib::log( LOG_ERROR, L"failed create mysql descriptor" );
		    return t_db_failed;
	    }
        return t_db_success;
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
		    return t_db_failed;
	    }
        return t_db_success;           
    }

    static e_status update( const t_db_table& dbtable, const t_db_field& dbfield, const t_db_value& dbvalue )
    {
        return t_db_failed;
    }

}                  