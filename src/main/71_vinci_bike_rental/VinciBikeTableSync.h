
/** VinciBikeTableSync class header.
	@file VinciBikeTableSync.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_VinciBikeTableSync_H__
#define SYNTHESE_VinciBikeTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace accounts
	{
		class TransactionPart;
	}

	namespace vinci
	{
		class VinciBike;
		class VinciContract;

		/** Vinci Bike SQLite table synchronizer.
			@ingroup m71LS refLS
		*/

		class VinciBikeTableSync : public db::SQLiteTableSyncTemplate<VinciBike>
		{
		public:
			static const std::string TABLE_COL_NUMBER;
			static const std::string TABLE_COL_MARKED_NUMBER;

			VinciBikeTableSync();

			/** VinciBike search.
				@param sqlite SQLite thread
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Founded accounts. 
				@author Hugues Romain
				@date 2006				
			*/
			static std::vector<boost::shared_ptr<VinciBike> > search(
				const std::string& id, const std::string& cadre
				, int first = 0, int number = -1
				, bool orderByNumber = true
				, bool orderByCadre = false
				, bool raisingOrder = true
				);

			/** Gets the contract corresponding to a current rent..
				@param bike The bike to search
				@return boost::shared_ptr<VinciContract> The rent contract. If the bike is not rented, then the method returns null.
				@author Hugues Romain
				@date 2007				
			*/
			static boost::shared_ptr<VinciContract> getRentContract(boost::shared_ptr<const VinciBike> bike);

			/** Gets the current rent service transaction part.
				@param bike The bike to search
				@return boost::shared_ptr<accounts::TransactionPart> The rent transaction part. If the bike is not rented, then th emethod returns a null shared pointer.
				@author Hugues Romain
				@date 2007				
			*/
			static boost::shared_ptr<accounts::TransactionPart> getRentTransactionPart(boost::shared_ptr<const VinciBike> bike);

		protected:

			/** Action to do on user creation.
			No action because the users are not permanently loaded in ram.
			*/
			void rowsAdded (db::SQLiteQueueThreadExec* sqlite,	
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			/** Action to do on user creation.
			Updates the users objects in the opened sessions.
			*/
			void rowsUpdated (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on user deletion.
			Closes the sessions of the deleted user.
			*/
			void rowsRemoved (db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};

	}
}

#endif 

