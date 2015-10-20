
/** CalendarTemplateElementTableSync class header.
	@file CalendarTemplateElementTableSync.h
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_CalendarTemplateElementTableSync_H__
#define SYNTHESE_CalendarTemplateElementTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "CalendarTemplateElement.h"

namespace synthese
{
	namespace calendar
	{
		//////////////////////////////////////////////////////////////////////////
		/// 31.10 Table : Calendar template element.
		/// @ingroup m31LS refLS
		/// @author Hugues Romain
		/// @date 2008
		//////////////////////////////////////////////////////////////////////////
		/// Object : CalendarTemplateElement
		/// See also : CalendarTemplateTableSync
		class CalendarTemplateElementTableSync:
			public db::DBDirectTableSyncTemplate<
				CalendarTemplateElementTableSync,
				CalendarTemplateElement>
		{
		public:
			static const std::string COL_CALENDAR_ID;
			static const std::string COL_RANK;
			static const std::string COL_MIN_DATE;
			static const std::string COL_MAX_DATE;
			static const std::string COL_INTERVAL;
			static const std::string COL_POSITIVE;
			static const std::string COL_INCLUDE_ID;

			virtual bool allowList( const server::Session* session ) const;

			/** CalendarTemplateElement search.
				@param env Environment to populate
				@param calendarId ID of the calendar which the searched elements must belong
				@param calendarIncludeId ID of the calendar that must be included by the returned elements
				@param first First CalendarTemplateElement object to answer
				@param number Number of CalendarTemplateElement objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param linkLevel level of link of the objects in the environment
				@return found objects
				@author Hugues Romain
				@date 2008-2010
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> calendarId,
				boost::optional<util::RegistryKeyType> calendarIncludeId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static void Shift(
				util::RegistryKeyType calendarId,
				size_t rank,
				int delta,
				boost::optional<db::DBTransaction&> transaction = boost::optional<db::DBTransaction&>()
			);

			static boost::optional<size_t> GetMaxRank(util::RegistryKeyType calendarId);

			static void Clean(
				util::RegistryKeyType calendarId,
				boost::optional<db::DBTransaction&> transaction = boost::optional<db::DBTransaction&>()
			);
		};
	}
}

#endif // SYNTHESE_CalendarTemplateElementTableSync_H__
