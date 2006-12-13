
#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"

#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/InterfaceTableSync.h"

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_NAME = "t024_interfaces";
		const std::string InterfaceTableSync::TABLE_COL_ID = "id";

		InterfaceTableSync::InterfaceTableSync()
			: SQLiteTableSync ( TABLE_NAME, true, true, db::TRIGGERS_ENABLED_CLAUSE )
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
		}


		void InterfaceTableSync::rowsUpdated( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}


		void InterfaceTableSync::rowsAdded( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				Interface* obj = new Interface(Conversion::ToLongLong(rows.getColumn(i,TABLE_COL_ID)));
				InterfaceModule::getInterfaces().add(obj);
			}
		}


		void InterfaceTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}



