
/** DisplayType class implementation.
	@file DisplayType.cpp

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

#include "DisplayType.h"

#include "MessageType.hpp"
#include "ParametersMap.h"
#include "Webpage.h"

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace departure_boards;
	using namespace util;

	CLASS_DEFINITION(DisplayType, "t036_display_types", 36)
	FIELD_DEFINITION_OF_OBJECT(DisplayType, "display_type_id", "display_type_ids")
	
	FIELD_DEFINITION_OF_TYPE(RowsNumber, "rows_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxStopsNumber, "max_stops_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TimeBetweenChecks, "time_between_checks", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayMainPage, "display_main_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayRowPage, "display_row_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayDestinationPage, "display_destination_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayTransferDestinationPage, "display_transfer_destination_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MonitoringParserPage, "monitoring_parser_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IsDisplayedMessagePage, "is_displayed_message_page_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MessageType, "message_type_id", SQL_INTEGER)


	namespace departure_boards
	{
		const string DisplayType::ATTR_ID = "id";
		const string DisplayType::ATTR_NAME = "name";
		const string DisplayType::ATTR_MESSAGE_TYPE_ID = "message_type_id";
		const string DisplayType::ATTR_ROWS_NUMBER = "rows_number";
		const string DisplayType::ATTR_MESSAGE_IS_DISPLAYED_PAGE_ID = "message_is_displayed_page_id";



		DisplayType::DisplayType(util::RegistryKeyType id)
		:	Registrable(id),
			Object<DisplayType, DisplayTypeSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(RowsNumber, 1),
					FIELD_VALUE_CONSTRUCTOR(MaxStopsNumber, 0),
					FIELD_VALUE_CONSTRUCTOR(TimeBetweenChecks, minutes(0)),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayMainPage),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayRowPage),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayDestinationPage),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayTransferDestinationPage),
					FIELD_DEFAULT_CONSTRUCTOR(MonitoringParserPage),
					FIELD_DEFAULT_CONSTRUCTOR(IsDisplayedMessagePage),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType)
			)	)
		{}
}	}
