#include "SQLiteDummyResult.h"


#include "SQLiteException.h"



using namespace synthese::util;
using namespace boost::posix_time;



namespace synthese
{
namespace db
{


SQLiteDummyResult::SQLiteDummyResult ()
{
}



SQLiteDummyResult::~SQLiteDummyResult ()
{

}




void 
SQLiteDummyResult::reset () const
{
}



bool 
SQLiteDummyResult::next () const
{
    return false;
}




int 
SQLiteDummyResult::getNbColumns () const
{
    return 0;
}



std::string
SQLiteDummyResult::getColumnName (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}




int
SQLiteDummyResult::getColumnIndex (const std::string& columnName) const
{
    throw SQLiteException ("Dummy result..."); 
}








SQLiteValue*
SQLiteDummyResult::getValue (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}




std::string
SQLiteDummyResult::getText (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}




int 
SQLiteDummyResult::getInt (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}




long
SQLiteDummyResult::getLong (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}






bool 
SQLiteDummyResult::getBool (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}



double 
SQLiteDummyResult::getDouble (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}





std::string 
SQLiteDummyResult::getBlob (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}





long long 
SQLiteDummyResult::getLongLong (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}




boost::posix_time::ptime
SQLiteDummyResult::getTimestamp (int column) const
{
    throw SQLiteException ("Dummy result..."); 
}



    







}
}



