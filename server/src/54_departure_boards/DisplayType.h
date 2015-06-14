////////////////////////////////////////////////////////////////////
/// DisplayType class header.
///	@file DisplayType.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayType_H__
#define SYNTHESE_DisplayType_H__

#include "Object.hpp"

#include "MessageType.hpp"
#include "MinutesField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "StringField.hpp"
#include "Webpage.h"

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace departure_boards
	{
		FIELD_SIZE_T(RowsNumber)
		FIELD_SIZE_T(MaxStopsNumber)
		FIELD_MINUTES(TimeBetweenChecks)
		FIELD_POINTER(DisplayMainPage, cms::Webpage)
		FIELD_POINTER(DisplayRowPage, cms::Webpage)
		FIELD_POINTER(DisplayDestinationPage, cms::Webpage)
		FIELD_POINTER(DisplayTransferDestinationPage, cms::Webpage)
		FIELD_POINTER(MonitoringParserPage, cms::Webpage)
		FIELD_POINTER(IsDisplayedMessagePage, cms::Webpage)
		FIELD_POINTER(MessageType, messages::MessageType)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(RowsNumber),
			FIELD(MaxStopsNumber),
			FIELD(TimeBetweenChecks),//!< Time between monitoring checks (0 = no value)
			FIELD(DisplayMainPage),
			FIELD(DisplayRowPage),
			FIELD(DisplayDestinationPage),
			FIELD(DisplayTransferDestinationPage),
			FIELD(MonitoringParserPage),
			FIELD(IsDisplayedMessagePage),
			FIELD(MessageType)
		> DisplayTypeSchema;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace messages
	{
		class MessageType;
	}

	namespace departure_boards
	{
		////////////////////////////////////////////////////////////////////
		/// Type of display screen, describing the input and output facilities.
		///	@ingroup m54
		///
		/// NULL pointers on interface attributes seems that the display type
		/// is not able to use the corresponding media :
		///		- display : No display can be produced
		///		- audio : No audio reading can be produced
		///		- monitoring : The display type cannot be monitored
		////////////////////////////////////////////////////////////////////
		class DisplayType:
			public virtual Object<DisplayType, DisplayTypeSchema>,
			public virtual util::Registrable
		{
		public:
			static const std::string ATTR_ID;
			static const std::string ATTR_NAME;
			static const std::string ATTR_MESSAGE_TYPE_ID;
			static const std::string ATTR_ROWS_NUMBER;
			static const std::string ATTR_DISPLAY_PAGE;
			static const std::string ATTR_MESSAGE_IS_DISPLAYED_PAGE_ID;

			/// Chosen registry class.
			typedef util::Registry<DisplayType>	Registry;

		private:

		public:


			////////////////////////////////////////////////////////////////////
			///	DisplayType constructor.
			///	@param key ID of the object
			///	@author Hugues Romain
			///	@date 2008
			///
			/// Initializes all interface pointers to NULL
			////////////////////////////////////////////////////////////////////
			DisplayType(util::RegistryKeyType key = 0);
		};
	}
}

#endif // SYNTHESE_DisplayType_H__
