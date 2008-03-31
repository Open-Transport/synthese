
/** DisplayScreenHasAlarmValueInterfaceElement class implementation.
	@file DisplayScreenHasAlarmValueInterfaceElement.cpp

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

#include <string>

#include "01_util/UId.h"
#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

#include "17_messages/Alarm.h"
#include "17_messages/Types.h"

#include "34_departures_table/DisplayScreenHasAlarmValueInterfaceElement.h"
#include "34_departures_table/DisplayScreenAlarmRecipient.h"
#include "34_departures_table/Types.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace messages;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, DisplayScreenHasAlarmValueInterfaceElement>::FACTORY_KEY("displayscreenalarmlevel");
	}

	namespace departurestable
	{
		string DisplayScreenHasAlarmValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object, const server::Request* request) const
		{
			const Alarm* alarm(static_cast<const ArrivalDepartureListWithAlarm*>(object)->alarm);
			
			stream << static_cast<int>(alarm ? alarm->getLevel() : ALARM_LEVEL_NO_ALARM);

			return string();
		}

		void DisplayScreenHasAlarmValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}
	}

}
