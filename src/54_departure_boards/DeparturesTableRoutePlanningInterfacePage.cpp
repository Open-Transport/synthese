
/** DeparturesTableRoutePlanningInterfacePage class implementation.
	@file DeparturesTableRoutePlanningInterfacePage.cpp

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

#include "DeparturesTableRoutePlanningInterfacePage.h"
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
		template<> const string FactorableTemplate<interfaces::InterfacePage, departure_boards::DeparturesTableRoutePlanningInterfacePage>::FACTORY_KEY("departurestablerouteplanning");
	}

	namespace departure_boards
	{
		const string DeparturesTableRoutePlanningInterfacePage::DATA_DISPLAY_SERVICE_NUMBER("display_service_number");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_DISPLAY_TRACK_NUMBER("display_track_number");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_WITH_TRANSFER("with_transfer");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_ORIGIN_ID("origin_id");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_ORIGIN_NAME("origin_name");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_TITLE("title");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_WIRING_CODE("wiring_code");
		const string DeparturesTableRoutePlanningInterfacePage::DATA_BLINKING_DELAY("blinking_delay");



		void DeparturesTableRoutePlanningInterfacePage::display(
			std::ostream& stream ,
			interfaces::VariablesMap& vars ,
			const std::string& title ,
			int wiringCode ,
			bool displayServiceNumber ,
			bool displayTrackNumber ,
			bool withTransfer,
			int blinkingDelay,
			bool displayClock,
			const pt::StopArea& place,
			const RoutePlanningListWithAlarm& rows ,
			const server::Request* request /*= NULL */ 
		) const	{
			ParametersVector pv;
			pv.push_back(title);
			pv.push_back(lexical_cast<string>(wiringCode));
			pv.push_back(lexical_cast<string>(displayServiceNumber));
			pv.push_back(lexical_cast<string>(displayTrackNumber));
			pv.push_back(lexical_cast<string>(withTransfer));
			pv.push_back(lexical_cast<string>(place.getKey()));
			pv.push_back(place.getFullName());
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


		DeparturesTableRoutePlanningInterfacePage::DeparturesTableRoutePlanningInterfacePage()
			: Registrable(0)
		{

		}
	}
}
 