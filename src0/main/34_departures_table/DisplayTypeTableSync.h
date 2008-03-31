
/** DisplayTypeTableSync class header.
	@file DisplayTypeTableSync.h

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

#ifndef SYNTHESE_DisplayTypeTableSync_H__
#define SYNTHESE_DisplayTypeTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DisplayType.h"

#include "02_db/SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayType;

		/** DisplayType table synchronizer.
			@ingroup m34LS refLS
		*/
		class DisplayTypeTableSync : public db::SQLiteRegistryTableSyncTemplate<DisplayTypeTableSync,DisplayType>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_INTERFACE_ID;
			static const std::string TABLE_COL_ROWS_NUMBER;
			static const std::string COL_MAX_STOPS_NUMBER;
	
			DisplayTypeTableSync();


			/** DisplayType search.
				(other search parameters)
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<DisplayType*> Founded currencies.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<DisplayType> > search(
				std::string exactName = std::string()
				, int first = 0
				, int number = UNKNOWN_VALUE
				, bool orderByName = true
				, bool raisingOrder = true
				);



		};
	}
}

#endif // SYNTHESE_DisplayTypeTableSync_H__
