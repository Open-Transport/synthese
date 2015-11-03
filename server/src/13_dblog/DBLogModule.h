
/** DBLogModule class header.
	@file DBLogModule.h

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

#ifndef SYNTHESE_DBLogModule_H__
#define SYNTHESE_DBLogModule_H__

#include <vector>
#include <utility>
#include <sstream>

#include "ModuleClassTemplate.hpp"
#include "DBLogEntry.h"

#include <boost/optional.hpp>

namespace synthese
{
	/**	@defgroup m13Actions 13 Actions
		@ingroup m13

		@defgroup m13Pages 13 Pages
		@ingroup m13

		@defgroup m13Functions 13 Functions
		@ingroup m13

		@defgroup m13Exceptions 13 Exceptions
		@ingroup m13

		@defgroup m13Alarm 13 Messages recipient
		@ingroup m13

		@defgroup m13LS 13 Table synchronizers
		@ingroup m13

		@defgroup m13Admin 13 Administration pages
		@ingroup m13

		@defgroup m13Rights 13 Rights
		@ingroup m13

		@defgroup m13Logs 13 DB Logs
		@ingroup m13

		@defgroup m13 13 Database stored applicative log
		@ingroup m1

		(Module documentation)

		@{
	*/

	/** 13 Database stored applicative log Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace dblog
	{
		/** Database stored applicative log module class.
		*/
		class DBLogModule:
			public server::ModuleClassTemplate<DBLogModule>
		{
		public:
			typedef std::vector<std::pair<boost::optional<int>, std::string> > Labels;
			static Labels	getEntryLevelLabels(bool withAll=false);
			static std::string					getEntryLevelLabel(const Level& level);
			static std::string					getEntryIcon(const Level& level);

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
