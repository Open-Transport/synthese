#include "02_db/SQLiteStatement.h"

#include "02_db/SQLiteHandle.h"

#include "02_db/SQLiteException.h"
#include "01_util/Conversion.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>



using namespace boost::posix_time;
using namespace synthese::util;




namespace synthese
{

namespace db
{



    
SQLiteStatement::SQLiteStatement (sqlite3_stmt* statement, const SQLData& sql)
    : _statement (statement)
    , _sql (sql)
{

}



SQLiteStatement::~SQLiteStatement ()
{
    sqlite3_finalize (_statement);
}





int 
SQLiteStatement::getParameterIndex (const std::string& parameterName) const
{
    return sqlite3_bind_parameter_index (_statement, parameterName.c_str ());

}




void 
SQLiteStatement::reset ()
{
    sqlite3_reset (_statement);
}



void 
SQLiteStatement::clearBindings ()
{
    sqlite3_clear_bindings (_statement);
}


void 
SQLiteStatement::bindParameterBlob (int index, const std::string& param)
{
    int retc = sqlite3_bind_blob (_statement, index, param.data (), param.length (), SQLITE_TRANSIENT);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error binding parameter " + Conversion::ToString (index) 
			       + " (error=" + Conversion::ToString (retc) + ")");
    }
}


void 
SQLiteStatement::bindParameterBlob (const std::string& name, const std::string& param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterBlob (index, param);

}




void 
SQLiteStatement::bindParameterInt (int index, int param)
{
    int retc = sqlite3_bind_int (_statement, index, param);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error binding parameter " + Conversion::ToString (index) 
			       + " (error=" + Conversion::ToString (retc) + ")");
    }
}


void 
SQLiteStatement::bindParameterInt (const std::string& name, int param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterInt (index, param);
}



void 
SQLiteStatement::bindParameterDouble (int index, double param)
{
    int retc = sqlite3_bind_double (_statement, index, param);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error binding parameter " + Conversion::ToString (index) 
			       + " (error=" + Conversion::ToString (retc) + ")");
    }
}


void 
SQLiteStatement::bindParameterDouble (const std::string& name, double param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterDouble (index, param);

}



void 
SQLiteStatement::bindParameterTimestamp (int index, const boost::posix_time::ptime& param)
{
    bindParameterText (index, to_simple_string (param.date()) + " " + to_simple_string(param.time_of_day()));
}


void 
SQLiteStatement::bindParameterTimestamp (const std::string& name, const boost::posix_time::ptime& param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterTimestamp (index, param);

}



void 
SQLiteStatement::bindParameterText (int index, const std::string& param)
{
    int retc = sqlite3_bind_text (_statement, index, param.c_str (), param.length (), SQLITE_TRANSIENT);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error binding parameter " + Conversion::ToString (index) 
			       + " (error=" + Conversion::ToString (retc) + ")");
    }
}


void 
SQLiteStatement::bindParameterText (const std::string& name, const std::string& param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterText (index, param);

}



void 
SQLiteStatement::bindParameterBool (int index, bool param)
{
    bindParameterInt (index, param ? 1 : 0); 
}


void 
SQLiteStatement::bindParameterBool (const std::string& name, bool param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterBool (index, param);
}



void 
SQLiteStatement::bindParameterLong (int index, long param)
{
    bindParameterText (index, Conversion::ToString (param)); 
}


void 
SQLiteStatement::bindParameterLong (const std::string& name, long param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterLong (index, param);
}



void 
SQLiteStatement::bindParameterLongLong (int index, long long param)
{
    int retc = sqlite3_bind_int64 (_statement, index, param);
    if (retc != SQLITE_OK)
    {
	throw SQLiteException ("Error binding parameter " + Conversion::ToString (index) 
			       + " (error=" + Conversion::ToString (retc) + ")");
    }
}


void 
SQLiteStatement::bindParameterLongLong (const std::string& name, long long param)
{
    int index = sqlite3_bind_parameter_index (_statement, name.c_str ());
    bindParameterLongLong (index, param);
}





sqlite3_stmt* 
SQLiteStatement::getStatement () const
{ 
    return _statement; 
}



}
}


		    
