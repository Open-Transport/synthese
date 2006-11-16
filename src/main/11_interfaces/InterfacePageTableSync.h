
#ifndef SYNTHESE_InterfacePageTableSync_H__
#define SYNTHESE_InterfacePageTableSync_H__

#include "02_db/SQLiteTableSync.h"
#include "11_interfaces/Interface.h"
#include <string>
#include <iostream>

namespace synthese
{
	namespace db
	{

		/** InterfacePageTableSync SQLite table synchronizer.
			@ingroup m11
		*/

		class InterfacePageTableSync : public SQLiteTableSync
		{
		private:
			static const std::string TABLE_NAME;
			static const std::string TABLE_COL_INTERFACE;
			static const std::string TABLE_COL_PAGE;
			static const std::string TABLE_COL_CONTENT;
			
			synthese::interfaces::Interface::Registry& _interfaces;

		public:

			/** Interface page SQLite table constructor.
			*/
			InterfacePageTableSync( synthese::interfaces::Interface::Registry& interfaces
				, const std::string& triggerOverrideClause );
			~InterfacePageTableSync ();

		protected:

			void rowsAdded (const SQLiteThreadExec* sqlite, 
				SQLiteSync* sync,
				const SQLiteResult& rows);

			void rowsUpdated (const SQLiteThreadExec* sqlite, 
				SQLiteSync* sync,
				const SQLiteResult& rows);

			void rowsRemoved (const SQLiteThreadExec* sqlite, 
				SQLiteSync* sync,
				const SQLiteResult& rows);

		};

	}
}

#endif // SYNTHESE_InterfacePageTableSync_H__
