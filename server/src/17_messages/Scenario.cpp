
/** Scenario class implementation.
	@file Scenario.cpp

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

#include "Scenario.h"

#include "MessagesSection.hpp"
#include "Alarm.h"
#include "ScenarioCalendar.hpp"


using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;

	FIELD_DEFINITION_OF_TYPE(Sections, "messages_section_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DataSourceLinksWithoutUnderscore, "datasource_links", SQL_TEXT)

	
	namespace messages
	{


		Scenario::Scenario(util::RegistryKeyType id)
			: Registrable(id)
		{
		}

		Scenario::~Scenario()
		{}


		void Scenario::addMessage(const Alarm& message) const
		{
			_messages.insert(&message);
		}
		
		
		void Scenario::removeMessage(const Alarm& message) const
		{
			if (_messages.find(&message) != _messages.end())
			{
				_messages.erase(&message);
			}
		}



		void Scenario::addSection(const MessagesSection& section) const
		{
			const_cast<Sections::Type&>(getSections()).insert(const_cast<MessagesSection*>(&section));
		}


}	}
