
#ifndef SYNTHESE_InterfacePageTableSync_H__
#define SYNTHESE_InterfacePageTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"

#include "11_interfaces/Interface.h"

namespace synthese
{
	namespace interfaces
	{

		/** InterfacePageTableSync SQLite table synchronizer.
			@ingroup m11
		*/
		class InterfacePageTableSync : public db::SQLiteTableSync
		{
		private:
			static const std::string TABLE_NAME;
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_INTERFACE;
			static const std::string TABLE_COL_PAGE;
			static const std::string TABLE_COL_CONTENT;
			
			synthese::interfaces::Interface::Registry& _interfaces;

		public:

			/** Interface page SQLite table constructor.
			*/
			InterfacePageTableSync(Interface::Registry& interfaces, const std::string& triggerOverrideClause );
			~InterfacePageTableSync ();

		protected:

			void rowsAdded (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsUpdated (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsRemoved (const db::SQLiteThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}

#endif // SYNTHESE_InterfacePageTableSync_H__
