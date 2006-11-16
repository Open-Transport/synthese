
#include "InterfacePageTableSync.h"
#include "11_interfaces/InterfacePage.h"
#include "02_db/SQLiteResult.h"
#include "01_util/Conversion.h"

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace db
	{
		const std::string InterfacePageTableSync::TABLE_NAME = "t023_interface_pages";
		const std::string InterfacePageTableSync::TABLE_COL_INTERFACE = "interface_id";
		const std::string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const std::string InterfacePageTableSync::TABLE_COL_CONTENT = "content";

		InterfacePageTableSync::InterfacePageTableSync(synthese::interfaces::Interface::Registry& interfaces
			, const std::string& triggerOverrideClause )
			: SQLiteTableSync ( TABLE_NAME, true, true, triggerOverrideClause )
			, _interfaces(interfaces)
		{
			addTableColumn(TABLE_COL_INTERFACE, "INTEGER", false);
			addTableColumn(TABLE_COL_PAGE, "TEXT", false);
			addTableColumn(TABLE_COL_CONTENT, "TEXT", true);
		}


		void InterfacePageTableSync::rowsUpdated( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				InterfacePage* page = 
					_interfaces.get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE)))
					->getPage(rows.getColumn(i, TABLE_COL_PAGE));
				page->parse( rows.getColumn(i, TABLE_COL_CONTENT ) );
			}
		}


		void InterfacePageTableSync::rowsAdded( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				InterfacePage* page = Factory<InterfacePage>::create( rows.getColumn(i,TABLE_COL_PAGE) );
				if ( page == NULL )
					page = new InterfacePage;
				page->clear();
				page->parse( rows.getColumn(i, TABLE_COL_CONTENT) );
				_interfaces.get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE )))
					->addPage(rows.getColumn(i, TABLE_COL_PAGE), page );
			}
		}


		void InterfacePageTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				_interfaces.get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE)))->removePage( rows.getColumn(i, TABLE_COL_PAGE) );
			}
		}
	}
}


