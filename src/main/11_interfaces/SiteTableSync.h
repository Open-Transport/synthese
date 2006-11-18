
#ifndef SYNTHESE_SiteTableSync_H__
#define SYNTHESE_SiteTableSync_H__




#include "02_db/SQLiteTableSync.h"
#include "11_interfaces/Site.h"
#include "11_interfaces/Interface.h"
#include "15_env/Environment.h"
#include <string>
#include <iostream>

namespace synthese
{
	namespace db
	{

		/** InterfaceTableSync SQLite table synchronizer.
		@ingroup m11
		*/

		class SiteTableSync : public SQLiteTableSync
		{
		private:
			static const std::string TABLE_NAME;
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_INTERFACE_ID;
			static const std::string TABLE_COL_ENVIRONMENT_ID;
			static const std::string TABLE_COL_START_DATE;
			static const std::string TABLE_COL_END_DATE;
			static const std::string TABLE_COL_ONLINE_BOOKING;
			static const std::string TABLE_COL_USE_OLD_DATA;
			static const std::string TABLE_COL_CLIENT_URL;

			synthese::interfaces::Site::Registry& _sites;
			const synthese::interfaces::Interface::Registry& _interfaces;
			const synthese::env::Environment::Registry& _environments;


		public:

			/** Site SQLite table constructor.
			*/
			SiteTableSync( synthese::interfaces::Site::Registry& sites
				, const std::string& triggerOverrideClause
				, const synthese::interfaces::Interface::Registry& interfaces
				, const synthese::env::Environment::Registry& environments);
			~SiteTableSync ();

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
#endif // SYNTHESE_SiteTableSync_H__
