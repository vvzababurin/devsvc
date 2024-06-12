#pragma once

#ifndef __WM_DEVDBLIB_H__
#define __WM_DEVDBLIB_H__

#include <string>
#include <vector>
#include <map>
#include <mysql.h>

#if _WIN32 || _WIN64
#pragma warning( push )
#pragma warning( disable : 4005 )
#endif

namespace wm_dblib
{
    static std::string dbsqlhost;
    static std::string dbsqluser;
    static std::string dbsqlpasswd;
    static std::string dbsqldb;

    ///////////////////////////////////////////////////	
    // typedef int t_db_success;
    // typedef int t_db_failed;
    ///////////////////////////////////////////////////	

    enum e_status {
        t_db_success,
        t_db_failed
    };

    class t_db_value : public std::vector<char> 
    {
    public:
        enum e_db_type 
        {
            integer,
            uinteger,
            string,
            wstring,
            blob
        };

        long long to_integer();
        unsigned long long to_uinteger();
        std::string to_string();
        std::wstring to_wstring();
        void* to_pointer( unsigned int& len );
    
        t_db_value from_integer( long long val );
        t_db_value from_uinteger( unsigned long long val );
        t_db_value from_string( std::string val );
        t_db_value from_wstring( std::wstring val );
        t_db_value from_pointer( void* val, unsigned int& len );
    };

    typedef std::string t_db_field;
    typedef std::string t_db_table;

    typedef std::map<t_db_field, t_db_value> t_db_row;
    typedef std::vector<t_db_row> t_db_rows;

    int create(const std::string& _dbsqlhost, const std::string& _dbsqluser, const std::string& _dbsqlpasswd, const std::string& _dbsqldb);
    void destroy();

    int connectdb( MYSQL* conn, const std::string &host, const std::string &db, const std::string &user, const std::string &passwd );

    static e_status updatedb( const t_db_table& dbtable, const t_db_field& dbfield, const t_db_value& dbvalue );
}

#if _WIN32 || _WIN64
#pragma warning( pop )
#endif

#endif // __DEVDBLIB_H_
