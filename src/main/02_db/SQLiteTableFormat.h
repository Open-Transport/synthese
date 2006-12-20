#ifndef SYNTHESE_DB_SQLITETABLEFORMAT_H
#define SYNTHESE_DB_SQLITETABLEFORMAT_H


#include <string>
#include <vector>





namespace synthese
{
	namespace db
	{

	    typedef struct {
		std::string name;
		std::string type;
		bool updatable;
	    } SQLiteTableColumnFormat;
	    

	    class SQLiteTableFormat 
	    {

	    private:

		std::vector<SQLiteTableColumnFormat> _columns;
		bool _hasNonUpdatableColumn;
		
	    protected:

	    public:
		    
		SQLiteTableFormat ();
		~SQLiteTableFormat ();

		bool hasTableColumn (const std::string& name) const;

		
		void addTableColumn (const std::string& name,
				     const std::string& type,
				     bool updatable);

		int getTableColumnCount () const;


		const SQLiteTableColumnFormat& getTableColumn (int index) const;
		bool hasNonUpdatableColumn () const;
		
	    };
	}
}
#endif

