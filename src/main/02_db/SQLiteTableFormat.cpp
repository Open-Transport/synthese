
#include "SQLiteTableFormat.h"



namespace synthese
{
namespace db
{



SQLiteTableFormat::SQLiteTableFormat ()
    : _hasNonUpdatableColumn (false)
{

}





SQLiteTableFormat::~SQLiteTableFormat ()
{

}




bool 
SQLiteTableFormat::hasTableColumn (const std::string& name) const
{
    for (std::vector<SQLiteTableColumnFormat>::const_iterator it = _columns.begin ();
	 it != _columns.end (); ++it)
    {
	if (it->name == name) return true;
    }
    return false;
}



		    


void 
SQLiteTableFormat::addTableColumn (const std::string& name,
				   const std::string& type,
				   bool updatable)
{
    SQLiteTableColumnFormat column;
    column.name = name;
    column.type = type;
    column.updatable = updatable;
    _columns.push_back (column);
    
    if (updatable == false)
    {
	_hasNonUpdatableColumn = true;
    }

}




int 
SQLiteTableFormat::getTableColumnCount () const
{
    return _columns.size ();
}





const SQLiteTableColumnFormat& 
SQLiteTableFormat::getTableColumn (int index) const
{
    return _columns.at (index);
}




bool 
SQLiteTableFormat::hasNonUpdatableColumn () const
{
    return _hasNonUpdatableColumn;
}






}
}

