
#include "InterfaceTableSync.h"
#include "02_db/SQLiteResult.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace db
	{
		const std::string InterfaceTableSync::TABLE_NAME = "t024_interfaces";
		const std::string InterfaceTableSync::TABLE_COL_ID = "id";

		InterfaceTableSync::InterfaceTableSync(synthese::interfaces::Interface::Registry& interfaces
			, const std::string& triggerOverrideClause )
			: SQLiteTableSync ( TABLE_NAME, true, true, triggerOverrideClause )
			, _interfaces(interfaces)
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
				_interfaces.add(obj);
			}
		}


		void InterfaceTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
		}
	}
}


