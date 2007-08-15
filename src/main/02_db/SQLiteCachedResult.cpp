#include "SQLiteCachedResult.h"


#include "SQLiteException.h"



using namespace synthese::util;
using namespace boost::posix_time;



namespace synthese
{
namespace db
{


SQLiteCachedResult::SQLiteCachedResult (const std::vector<std::string>& columnNames)
    : _pos (-1)
    , _columnNames (columnNames)
{

}





SQLiteCachedResult::SQLiteCachedResult (const SQLiteResultSPtr& result)
    : _pos (-1)
{
    for (int i=0; i<result->getNbColumns (); ++i)
    {
	_columnNames.push_back (result->getColumnName (i));
    }

    while (result->next ())
    {
	addRow (result->getRow ());
    }
}



SQLiteCachedResult::~SQLiteCachedResult ()
{

}




void 
SQLiteCachedResult::reset () const
{
    _pos = -1;
}



bool 
SQLiteCachedResult::next () const
{
    ++_pos;
    return (_pos < _rows.size ());
}




int 
SQLiteCachedResult::getNbColumns () const
{
    return _columnNames.size ();
}



std::string
SQLiteCachedResult::getColumnName (int column) const
{
    return _columnNames.at (column);
}




SQLiteValueSPtr
SQLiteCachedResult::getValue (int column) const
{
    return _rows.at (_pos).at (column);
}



SQLiteResultRow 
SQLiteCachedResult::getRow () const
{
    return _rows.at (_pos);
}
    


void 
SQLiteCachedResult::addRow (const SQLiteResultRow& row)
{
    _rows.push_back (row);
}






}
}



