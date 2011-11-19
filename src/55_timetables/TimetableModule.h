
/** TimetableModule class header.
	@file TimetableModule.h

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

#ifndef SYNTHESE_timetables_TimetableModule_h__
#define SYNTHESE_timetables_TimetableModule_h__

#include "ModuleClassTemplate.hpp"
#include "Registry.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	/**	@defgroup m55Exceptions 55.01 Exceptions
		@ingroup m55

		@defgroup m55LS 55.10 Table synchronizers
		@ingroup m55

		@defgroup m55Pages 55.11 Interface pages
		@ingroup m55

		@defgroup m55Rights 55.12 Rights
		@ingroup m55

		@defgroup m55Logs 55.13 DB Logs
		@ingroup m55

		@defgroup m55Admin 55.14 Administration pages
		@ingroup m55

		@defgroup m55Actions 55.15 Actions
		@ingroup m55

		@defgroup m55Functions 55.15 Functions
		@ingroup m55

		@defgroup m55Alarm 55.17 Messages recipient
		@ingroup m55

		@defgroup m55 55 Timetables generator
		@ingroup m5



		@{
	*/

	/** 55 Timetables generator Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace timetables
	{
		/** TimetableModule class.
		*/
		class TimetableModule:
			public server::ModuleClassTemplate<TimetableModule>
		{
		public:

			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > TimetableContainersLabels;

			/** Labels list containing each container timetable, indicating the full path in the folder tree.
				@param folderId id of the main parent folder (optional)
				@param prefix text to add at the beginning of each item (optional)
				@param forbiddenFolderId id of a folder which must not be present in the result
				@return The list
				@author Hugues Romain
				@date 2008
			*/
			static TimetableContainersLabels GetTimetableContainersLabels(
				util::RegistryKeyType folderId = 0,
				std::string prefix = std::string(),
				boost::optional<util::RegistryKeyType> forbiddenFolderId = boost::optional<util::RegistryKeyType>()
			);
		};
	}
}

#endif // SYNTHESE_timetables_TimetableModule_h__
