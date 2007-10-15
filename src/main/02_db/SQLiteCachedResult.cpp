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


SQLiteCachedResult::SQLiteCachedResult ()
    : _pos (-1)

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
    for (std::vector<SQLiteResultRow>::iterator it = _rows.begin ();
	 it != _rows.end (); ++it) 
    {
	SQLiteResultRow& row = *it;
	for (SQLiteResultRow::iterator it2 = row.begin ();
	     it2 != row.end (); ++it2) 
	{
	    delete *it2;
	}

    }
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




SQLiteValue*
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



void 
SQLiteCachedResult::addRow (int nbColumns, char** values, char** columns)
{
    if (_columnNames.size () == 0) 
    {
	for (int i=0; i<nbColumns; ++i) _columnNames.push_back (columns[i]);
    }
    SQLiteResultRow row;
    for (int i=0; i<nbColumns; ++i) 
    {
	if (values[i] == 0)
	{
	    row.push_back (new SQLiteValue (""));
	}
	else
	{
	    row.push_back (new SQLiteValue (values[i]));
	}
    }
    addRow (row);
}





}
}



