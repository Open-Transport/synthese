
/** CalendarTemplateTableSync class header.
	@file CalendarTemplateTableSync.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_CalendarTemplateTableSync_H__
#define SYNTHESE_CalendarTemplateTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

#include "CalendarTemplate.h"

namespace synthese
{
	namespace timetables
	{
		/** CalendarTemplate table synchronizer.
			@ingroup m55LS refLS
		*/
		class CalendarTemplateTableSync
		:	public db::SQLiteRegistryTableSyncTemplate<CalendarTemplateTableSync,CalendarTemplate>
		{
		public:
			static const std::string COL_TEXT;
			
			CalendarTemplateTableSync();


			/** CalendarTemplate search.
				(other search parameters)
				@param first First CalendarTemplate object to answer
				@param number Number of CalendarTemplate objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<CalendarTemplate> Founded CalendarTemplate objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				// other search parameters ,
				int first = 0, int number = 0,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_CalendarTemplateTableSync_H__
