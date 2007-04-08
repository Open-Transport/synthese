
#ifndef SYNTHESE_VinciRateTableSync_H__
#define SYNTHESE_VinciRateTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciRate;

		/** Vinci Rate SQLite table synchronizer.
			@ingroup m71LS refLS
		*/
		class VinciRateTableSync : public db::SQLiteTableSyncTemplate<VinciRate>
		{
		private:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_VALIDITY_DURATION;
			static const std::string TABLE_COL_START_FINANCIAL_PRICE;
			static const std::string TABLE_COL_START_TICKETS_PRICE;
			static const std::string TABLE_COL_END_FINANCIAL_PRICE;
			static const std::string TABLE_COL_END_TICKETS_PRICE;
			static const std::string TABLE_COL_FIRST_PENALTY;
			static const std::string TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION;
			static const std::string TABLE_COL_RECURRING_PENALTY;
			static const std::string TABLE_COL_RECURRING_PENALTY_PERIOD;

			friend class db::SQLiteTableSyncTemplate<VinciRate>;
			
		public:
			VinciRateTableSync();
			~VinciRateTableSync ();

			/** VinciRate search.
			@param sqlite SQLite thread
			@param first First user to answer
			@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			@return Founded rates. 
			@author Hugues Romain
			@date 2006	
			*/
			static std::vector<VinciRate*> search(
				int first = 0, int number = -1);


		protected:

			/** Action to do on user creation.
			No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user creation.
			Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on user deletion.
			Closes the sessions of the deleted user.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}

#endif 

