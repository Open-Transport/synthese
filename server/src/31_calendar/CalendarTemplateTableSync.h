
/** CalendarTemplateTableSync class header.
	@file CalendarTemplateTableSync.h
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

#ifndef SYNTHESE_CalendarTemplateTableSync_H__
#define SYNTHESE_CalendarTemplateTableSync_H__


#include <boost/optional.hpp>

#include "DBDirectTableSyncTemplate.hpp"
#include "CalendarTemplate.h"

namespace synthese
{
	namespace calendar
	{
		//////////////////////////////////////////////////////////////////////////
		/// 31.10 Table : Calendar templates.
		///	@ingroup m31LS refLS
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t054_calendar_templates
		/// Corresponding class : CalendarTemplate
		class CalendarTemplateTableSync:
			public db::DBDirectTableSyncTemplate<
				CalendarTemplateTableSync,
				CalendarTemplate
			>
		{
		public:

			virtual bool allowList( const server::Session* session ) const;

			/** CalendarTemplate search.
				@param env Environment to populate
				@param first First CalendarTemplate object to answer
				@param number Number of CalendarTemplate objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found CalendarTemplate objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				boost::optional<util::RegistryKeyType> forbiddenId = boost::optional<util::RegistryKeyType>(),
				bool orderByName = true,
				bool raisingOrder = true,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>()
			);


			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > CalendarTemplatesList;

			//////////////////////////////////////////////////////////////////////////
			/// Generates a list of the calendar templates.
			/// @param zeroName if non empty, a zero choice is added at the top of the list, named by the parameter value
			/// @return List of the calendar templates, to be used at parameter of HTMLForm::getSelectInput()
			static CalendarTemplatesList GetCalendarTemplatesList(
				CalendarTemplatesList::value_type::second_type zeroName = CalendarTemplatesList::value_type::second_type(),
				boost::optional<CalendarTemplatesList::value_type::first_type> idToAvoid = boost::optional<CalendarTemplatesList::value_type::first_type>(),
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>()
			);
		};
	}
}

#endif // SYNTHESE_CalendarTemplateTableSync_H__
