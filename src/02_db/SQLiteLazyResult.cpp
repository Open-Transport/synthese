#include "SQLiteLazyResult.h"

#include "01_util/Conversion.h"

#include "02_db/SQLiteException.h"


#include <iomanip>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>



using namespace synthese::util;
using namespace boost::posix_time;


namespace synthese
{
namespace db
{


SQLiteLazyResult::SQLiteLazyResult (SQLiteStatementSPtr statement)
    : _statement (statement)
    , _pos (-1) 
{
    // std::cerr << "new lazy " << _statement->getSQL () << std::endl;
}



SQLiteLazyResult::~SQLiteLazyResult ()
{
    // std::cerr << "del lazy " << _statement->getSQL () << std::endl;
}




void 
SQLiteLazyResult::reset () const
{
    int retc = sqlite3_reset (_statement->getStatement ());
    if (retc == SQLITE_OK) 
    {
	_pos = -1;
    }
    else
    {
	throw SQLiteException ("Error while resetting statement (error=" 
			       + Conversion::ToString (retc) + ")");
    }
    
}



bool 
SQLiteLazyResult::next () const
{
    int retc = sqlite3_step (_statement->getStatement ());
    if (retc == SQLITE_ROW) 
    {
	++_pos;
	return true;
    }
    else if (retc == SQLITE_DONE)
    {
	++_pos;
	return false;
    }
    else
    {
	throw SQLiteException ("Error while stepping through statement (error=" 
			       + Conversion::ToString (retc) + ")");
    }
}




int 
SQLiteLazyResult::getNbColumns () const
{
    return sqlite3_column_count (_statement->getStatement ());
}



std::string
SQLiteLazyResult::getColumnName (int column) const
{
    return std::string (sqlite3_column_name (_statement->getStatement (), column));
}











SQLiteValue*
SQLiteLazyResult::getValue (int column) const
{
    if (_pos == -1) throw SQLiteException ("Not inside result (use next ())...");
    return new SQLiteValue (sqlite3_column_value (_statement->getStatement (), column));
}







}
}



