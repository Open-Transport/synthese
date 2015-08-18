/** IneoTerminusLog class header.
	@file IneoTerminusLog.hpp
	@author Camille Hue
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_messages_IneoTerminusLog_hpp__
#define SYNTHESE_messages_IneoTerminusLog_hpp__

#include "DBLogTemplate.h"
#include "XmlParser.h"

#include <string>

namespace synthese
{
	namespace ineo_terminus
	{
		class IneoTerminusLog:
			public dblog::DBLogTemplate<IneoTerminusLog>
		{
		public:
			/// Log name
			std::string getName() const;

			/// Terminus log columns
			DBLog::ColumnsVector getColumnNames() const;

			/// Converts registry key into object name
			virtual std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;

			/**
				Add an entry for a IneoTerminus error message.
			*/
			static void AddIneoTerminusErrorMessageEntry(
				XMLNode node
			);

		private:

		};

	} /* namespace ineo_terminus */
} /* namespace synthese */

#endif /* SYNTHESE_messages_IneoTerminusLog_hpp__ */
