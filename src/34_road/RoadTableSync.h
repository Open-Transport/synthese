
/** RoadTableSync class header.
	@file RoadTableSync.h

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

#ifndef SYNTHESE_RoadTableSync_H__
#define SYNTHESE_RoadTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "Road.h"
#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace road
	{
		/** Road table synchronizer.
			@ingroup m34LS refLS
		*/
		class RoadTableSync
		:	public db::SQLiteRegistryTableSyncTemplate<RoadTableSync,Road>
		{
		public:
			/** Roads table :
			- on insert : 
			- on update : 
			- on delete : X
			*/
			static const std::string COL_NAME;
			static const std::string COL_CITYID;
			static const std::string COL_ROADTYPE;
			static const std::string COL_FAREID;
			static const std::string COL_BIKECOMPLIANCEID;
			static const std::string COL_HANDICAPPEDCOMPLIANCEID;
			static const std::string COL_PEDESTRIANCOMPLIANCEID;
			static const std::string COL_RESERVATIONRULEID;
			static const std::string COL_VIAPOINTS;
			// list of chunk ids
			
			RoadTableSync();


			/** Road search.
				(other search parameters)
				@param first First Road object to answer
				@param number Number of Road objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<Road> Founded Road objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_RoadTableSync_H__
