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
    static std::string db_sqlhost;
    static std::string db_sqluser;
    static std::string db_sqlpasswd;
    static std::string db_sqldb;

    ///////////////////////////////////////////////////	
    // typedef int t_db_success;
    // typedef int t_db_failed;
    ///////////////////////////////////////////////////	

    enum e_status {
        db_success = 0,
        db_failed
    };

    class db_value : public std::vector<char> 
    {
    public:
        enum db_type 
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
    
        db_value from_integer( long long val );
        db_value from_uinteger( unsigned long long val );
        db_value from_string( std::string val );
        db_value from_wstring( std::wstring val );
        db_value from_pointer( void* val, unsigned int& len );
    };

    typedef std::string db_field;
    typedef std::string db_table;

    typedef std::map<db_field, db_value> db_row;
    typedef std::vector<db_row> db_rows;

    int create(const std::string& _db_sqlhost, const std::string& _db_sqluser, const std::string& _db_sqlpasswd, const std::string& _db_sqldb);
    void destroy();

    int connectdb( MYSQL* conn, const std::string &host, const std::string &db, const std::string &user, const std::string &passwd );

    static e_status updatedb( const db_table& dbtable, const db_field& dbfield, const db_value& dbvalue );
}

#if _WIN32 || _WIN64
#pragma warning( pop )
#endif

#endif // __DEVDBLIB_H_
