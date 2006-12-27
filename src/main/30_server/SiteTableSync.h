
#ifndef SYNTHESE_SiteTableSync_H__
#define SYNTHESE_SiteTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"

namespace synthese
{
	namespace server
	{

		/** InterfaceTableSync SQLite table synchronizer.
		@ingroup m11
		*/

		class SiteTableSync : public db::SQLiteTableSync
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

		public:

			/** Site SQLite table constructor.
			*/
			SiteTableSync();
			~SiteTableSync ();

		protected:

			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_SiteTableSync_H__

