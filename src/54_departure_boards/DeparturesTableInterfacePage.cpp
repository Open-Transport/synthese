
/** DeparturesTableInterfacePage class implementation.
	@file DeparturesTableInterfacePage.cpp

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

#include "DeparturesTableInterfacePage.h"
#include "StopArea.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::InterfacePage, departure_boards::DeparturesTableInterfacePage>::FACTORY_KEY("departurestable");
	}

	namespace departure_boards
	{
		const string DeparturesTableInterfacePage::DATA_DISPLAY_SERVICE_NUMBER("display_service_number");
		const string DeparturesTableInterfacePage::DATA_DISPLAY_TEAM("display_team");
		const string DeparturesTableInterfacePage::DATA_DISPLAY_TRACK_NUMBER("display_track_number");
		const string DeparturesTableInterfacePage::DATA_INTERMEDIATE_STOPS_NUMBER("intermediate_stops_number");
		const string DeparturesTableInterfacePage::DATA_STOP_NAME("stop_name");
		const string DeparturesTableInterfacePage::DATA_TITLE("title");
		const string DeparturesTableInterfacePage::DATA_WIRING_CODE("wiring_code");
		const string DeparturesTableInterfacePage::DATA_BLINKING_DELAY("blinking_delay");
		const string DeparturesTableInterfacePage::DATA_DISPLAY_CLOCK("display_clock");

		void DeparturesTableInterfacePage::display( std::ostream& stream
			, VariablesMap& vars
			, const std::string& title
			, int wiringCode
			, bool displayServiceNumber
			, bool displayTrackNumber
			, bool displayTeam,
			optional<size_t> intermediatesStopsToDisplay,
			int blinkingDelay,
			bool displayClock
			, const pt::StopArea* place
			, const ArrivalDepartureList& rows
			, const server::Request* request /*= NULL*/ ) const
		{
			ParametersVector pv;
			pv.push_back(title);
			pv.push_back(lexical_cast<string>(wiringCode));
			pv.push_back(lexical_cast<string>(displayServiceNumber));
			pv.push_back(lexical_cast<string>(displayTrackNumber));
			pv.push_back(intermediatesStopsToDisplay ? lexical_cast<string>(*intermediatesStopsToDisplay) : string());
			pv.push_back(lexical_cast<string>(displayTeam));
			pv.push_back(place->getFullName());
			pv.push_back(lexical_cast<string>(blinkingDelay));
			pv.push_back(lexical_cast<string>(displayClock));

			InterfacePage::_display(
				stream
				, pv
				, vars
				, (const void*) &rows
				, request);
		}



		void DeparturesTableInterfacePage::display(
			std::ostream& stream ,
			interfaces::VariablesMap& vars ,
			const std::string& title ,
			int wiringCode ,
			bool displayServiceNumber ,
			bool displayTrackNumber ,
			int blinkingDelay,
			bool displayClock,
			const pt::StopArea* place ,
			const RoutePlanningList& rows ,
			const server::Request* request /*= NULL */
		) const	{
			ParametersVector pv;
			pv.push_back(title);
			pv.push_back(lexical_cast<string>(wiringCode));
			pv.push_back(lexical_cast<string>(displayServiceNumber));
			pv.push_back(lexical_cast<string>(displayTrackNumber));
			pv.push_back(string());
			pv.push_back(string());
			pv.push_back(place->getFullName());
			pv.push_back(lexical_cast<string>(blinkingDelay));
			pv.push_back(lexical_cast<string>(displayClock));

			InterfacePage::_display(
				stream
				, pv
				, vars
				, (const void*) &rows
				, request
			);
		}


		DeparturesTableInterfacePage::DeparturesTableInterfacePage()
			: Registrable(0)
		{

		}
	}
}

