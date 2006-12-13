
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/SQLiteResult.h"

#include "11_interfaces/InterfaceModule.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/InterfacePageTableSync.h"

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace interfaces
	{
		const std::string InterfacePageTableSync::TABLE_NAME = "t023_interface_pages";
		const std::string InterfacePageTableSync::TABLE_COL_ID = "id";
		const std::string InterfacePageTableSync::TABLE_COL_INTERFACE = "interface_id";
		const std::string InterfacePageTableSync::TABLE_COL_PAGE = "page_code";
		const std::string InterfacePageTableSync::TABLE_COL_CONTENT = "content";

		InterfacePageTableSync::InterfacePageTableSync()
			: SQLiteTableSync ( TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_INTERFACE, "INTEGER", false);
			addTableColumn(TABLE_COL_PAGE, "TEXT", false);
			addTableColumn(TABLE_COL_CONTENT, "TEXT", true);
		}


		void InterfacePageTableSync::rowsUpdated( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				InterfacePage* page = 
					InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE)))
					->getPage(rows.getColumn(i, TABLE_COL_PAGE));
				page->parse( rows.getColumn(i, TABLE_COL_CONTENT ) );
			}
		}


		void InterfacePageTableSync::rowsAdded( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				Interface* interf;
				InterfacePage* page;
				
				// Search the specified interface
				try
				{
					interf = InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE )));
				}
				catch (Interface::RegistryKeyException e)
				{
					Log::GetInstance().warn("Corrupted data on "+ TABLE_NAME +" table : Interface not found", e);
					continue;
				}

				try
				{
					// Registered interface page;
					page = Factory<InterfacePage>::create( rows.getColumn(i,TABLE_COL_PAGE) );
				}
				catch (FactoryException<InterfacePage> e)
				{
					// Free interface page
					page = new InterfacePage;
				}

				page->parse( rows.getColumn(i, TABLE_COL_CONTENT) );
				interf->addPage(rows.getColumn(i, TABLE_COL_PAGE), page );
			}
		}


		void InterfacePageTableSync::rowsRemoved( const SQLiteThreadExec* sqlite,  SQLiteSync* sync, const SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				InterfaceModule::getInterfaces().get(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_INTERFACE)))->removePage( rows.getColumn(i, TABLE_COL_PAGE) );
			}
		}
	}
}



