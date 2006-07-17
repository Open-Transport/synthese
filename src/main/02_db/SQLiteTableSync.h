#ifndef SYNTHESE_DB_SQLITETABLESYNC_H
#define SYNTHESE_DB_SQLITETABLESYNC_H



#include <string>
#include <vector>
#include <iostream>

#include <01_util/UId.h>


namespace synthese
{
namespace db
{

    class SQLiteSync;
    class SQLiteResult;
    class SQLiteThreadExec;

/** 
    Base class for an SQLite table synchronizer.
    By convention, the table name must always start with the t letter
    followed by a unique 3 digits integer (SQLite does not allow ids starting with number).

@ingroup m02
*/

    typedef std::vector<std::pair<std::string, std::string> > SQLiteTableFormat;

class SQLiteTableSync
{
 private:

    const std::string _tableName;
    SQLiteTableFormat _tableFormat;

 public:

    SQLiteTableSync ( const std::string& tableName );

    ~SQLiteTableSync ();

    const std::string& getTableName () const;

    /** Returns the unique integer identifying a table.
     */
    int getTableId () const;

    const SQLiteTableFormat& getTableFormat () const;

    uid encodeUId (int gridId, int gridNodeId, long objectId);

    /** This method is called when the synchronizer is created
	to sychronize it with pre-existing data in db.
    */
    void firstSync (const synthese::db::SQLiteThreadExec* sqlite, 
		    synthese::db::SQLiteSync* sync);

    virtual void rowsAdded (const SQLiteThreadExec* sqlite, 
			    SQLiteSync* sync,
			    const SQLiteResult& rows) = 0;

    virtual void rowsUpdated (const SQLiteThreadExec* sqlite, 
			      SQLiteSync* sync,
			      const SQLiteResult& rows) = 0;

    virtual void rowsRemoved (const SQLiteThreadExec* sqlite, 
			      SQLiteSync* sync,
			      const SQLiteResult& rows) = 0;

 protected:

    void addTableColumn (const std::string& columnName, const std::string& columnType);

 private:


};




}

}
#endif
