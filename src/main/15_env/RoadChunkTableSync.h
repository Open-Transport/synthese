
/** RoadChunkTableSync class header.
	@file RoadChunkTableSync.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_RoadChunkTableSync_H__
#define SYNTHESE_RoadChunkTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class RoadChunk;

		/** RoadChunk table synchronizer.
			@ingroup m15LS refLS
		*/
		class RoadChunkTableSync : public db::SQLiteTableSyncTemplate<RoadChunk>
		{
		public:
			/** Road chunks table :
			- on insert : 
			- on update : 
			- on delete : X
			*/
			static const std::string COL_ADDRESSID;
			static const std::string COL_RANKINPATH;
			static const std::string COL_ISDEPARTURE;
			static const std::string COL_ISARRIVAL;
			static const std::string COL_VIAPOINTS;  // list of ids
			
			RoadChunkTableSync();


			/** RoadChunk search.
				(other search parameters)
				@param first First RoadChunk object to answer
				@param number Number of RoadChunk objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<RoadChunk> Founded RoadChunk objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<RoadChunk> > search(
				// other search parameters ,
				int first = 0, int number = 0);


		protected:

			/** Action to do on RoadChunk creation.
				This method loads a new object in ram.
			*/
			void rowsAdded (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool);

			/** Action to do on RoadChunk creation.
				This method updates the corresponding object in ram.
			*/
			void rowsUpdated (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			/** Action to do on RoadChunk deletion.
				This method deletes the corresponding object in ram and runs 
				all necessary cleaning actions.
			*/
			void rowsRemoved (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};
	}
}

#endif // SYNTHESE_RoadChunkTableSync_H__
