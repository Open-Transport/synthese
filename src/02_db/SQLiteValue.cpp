#include "SQLiteValue.h"

#include "01_util/Conversion.h"

#include "SQLiteException.h"

#include <iomanip>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>



using namespace synthese::util;
using namespace boost::posix_time;



namespace synthese
{
namespace db
{


SQLiteValue::SQLiteValue (sqlite3_value* value)
    : _value ("")
{
    int type (sqlite3_value_type (value));
    switch (type)
    {
    case SQLITE_INTEGER:
	_value = Conversion::ToString (sqlite3_value_int64 (value));
	break;
    case SQLITE_FLOAT:
	_value = Conversion::ToString (sqlite3_value_double (value));
	break;
    case SQLITE_BLOB:
	_value.assign ((const char*) sqlite3_value_blob (value), sqlite3_value_bytes (value));
	break;
    case SQLITE_NULL:
	_value = "";
	break;
    case SQLITE_TEXT:
	_value.assign ((const char*) sqlite3_value_text (value));
	break;
	

    }
}



SQLiteValue::SQLiteValue (const std::string& value)
    : _value ()
{
    _value.assign (value.data ());
}



SQLiteValue::~SQLiteValue ()
{
}



const std::string&
SQLiteValue::getText () const
{
    return (_value);
}

    
    
    
int 
SQLiteValue::getInt () const
{
    return Conversion::ToInt (getText ());
}
    
    

long
SQLiteValue::getLong () const
{
    return Conversion::ToLong (getText ());
}





bool 
SQLiteValue::getBool () const
{
    return Conversion::ToBool (getText ());
}
    



double 
SQLiteValue::getDouble () const
{
    return Conversion::ToDouble (getText ());
}




const std::string& 
SQLiteValue::getBlob () const
{
    return _value;
}





long long 
SQLiteValue::getLongLong () const
{
    return Conversion::ToLongLong (getText ());
}





boost::posix_time::ptime
SQLiteValue::getTimestamp () const
{
    return from_iso_string (getText ());
}


    
boost::logic::tribool 
SQLiteValue::getTribool () const
{
    return Conversion::ToTribool (getText ());
}



}
}



