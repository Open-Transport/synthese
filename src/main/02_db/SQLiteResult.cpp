#include "SQLiteResult.h"

#include "SQLiteException.h"


namespace synthese
{
namespace db
{


SQLiteResult::SQLiteResult ()
    : _nbColumns (0)
    , _nbRows (0)
{
}



SQLiteResult::~SQLiteResult () throw ()
{

}



int 
SQLiteResult::getNbColumns () const
{
    return _nbColumns;
}




int 
SQLiteResult::getNbRows () const
{
    return _nbRows;
}



const std::string& 
SQLiteResult::getColumnName (int column) const
{
    return _columnNames.at (column);
}



int
SQLiteResult::getColumnIndex (const std::string& columnName) const
{
    for (int i=0; i<(int) _columnNames.size (); ++i)
    {
	if (_columnNames.at(i) == columnName) return i;
    }
    return -1;
}




const std::string& 
SQLiteResult::getColumn (int row, int column) const
{
    return _values.at (row).at (column);
}



const std::string& 
SQLiteResult::getColumn (int row, const std::string& name) const
{
    int index = getColumnIndex (name);
    if (index == -1) throw SQLiteException ("No such column " + name);
    return getColumn (row, index);
}





void 
SQLiteResult::addRow (int nbColumns, char** values, char** columns)
{
    ++_nbRows;
    _nbColumns = nbColumns;

    Row row;
    if (_columnNames.size () == 0) 
    {
	for (int j=0; j<nbColumns; ++j)
	{
	    _columnNames.push_back (columns[j]);
	}
    }

    for (int i=0; i<nbColumns; ++i)
    {
	row.push_back (values[i]);
    }
    _values.push_back (row);

}




}
}


