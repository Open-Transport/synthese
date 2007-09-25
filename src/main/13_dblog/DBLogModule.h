
/** DBLogModule class header.
	@file DBLogModule.h

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

#ifndef SYNTHESE_DBLogModule_H__
#define SYNTHESE_DBLogModule_H__

#include <vector>
#include <utility>

#include "01_util/ModuleClass.h"

#include "13_dblog/DBLogEntry.h"

namespace synthese
{
	/** @defgroup m13 13 Database stored applicative log.
		@ingroup m1
		@{
	*/

	/** 13 database stored applicative log namespace.
	*/
	namespace dblog
	{
		/** Database stored applicative log module class.
		*/
		class DBLogModule : public util::ModuleClass
		{
		public:
			static std::vector<std::pair<int, std::string> >	getEntryLevelLabels(bool withAll=false);
			static std::string					getEntryLevelLabel(const DBLogEntry::Level& level);
			static std::string					getEntryIcon(const DBLogEntry::Level& level);

			template<class T>
			static void appendToLogIfChange(std::stringstream& s, const std::string& label, const T& oldValue, const T& newValue)
			{
				if (oldValue != newValue)
					s << " - " << label << " : " << oldValue << " => " << newValue;
			}
		};
	}

	/** @} */
}

#endif // SYNTHESE_DBLogModule_H__
