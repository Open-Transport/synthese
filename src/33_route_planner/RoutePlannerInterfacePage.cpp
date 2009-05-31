
/** RoutePlannerInterfacePage class implementation.
	@file RoutePlannerInterfacePage.cpp

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

#include "RoutePlannerInterfacePage.h"

#include "HourPeriod.h"
#include "Site.h"

#include "City.h"
#include "Place.h"
#include "AccessParameters.h"
#include "PTConstants.h"
#include "DateTimeInterfacePage.h"
#include "Interface.h"

#include "Date.h"
#include "NamedPlace.h"
#include "Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace transportwebsite;
	using namespace geography;
	using namespace graph;

	template<> const string util::FactorableTemplate<InterfacePage,routeplanner::RoutePlannerInterfacePage>::FACTORY_KEY("route_planner");

	namespace routeplanner
	{
		const string RoutePlannerInterfacePage::DATA_INTERNAL_DATE("internal_date");
		const string RoutePlannerInterfacePage::DATA_IS_HOME("is_home");
		const string RoutePlannerInterfacePage::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const string RoutePlannerInterfacePage::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string RoutePlannerInterfacePage::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const string RoutePlannerInterfacePage::DATA_BIKE_FILTER("bike_filter");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_PLACE_ID("destination_place_id");
		const string RoutePlannerInterfacePage::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
		const string RoutePlannerInterfacePage::DATA_PERIOD_ID("period_id");
		const string RoutePlannerInterfacePage::DATA_DATE("date");
		const string RoutePlannerInterfacePage::DATA_PERIOD("period");
		const string RoutePlannerInterfacePage::DATA_SOLUTIONS_NUMBER("solutions_number");
		const string RoutePlannerInterfacePage::DATA_ACCESSIBILITY_CODE("accessibility_code");
		const string RoutePlannerInterfacePage::DATA_SITE_ID("site_id");
		const string RoutePlannerInterfacePage::DATA_IS_SAME_PLACES("is_same_places");

		void RoutePlannerInterfacePage::display(
			ostream& stream
			, VariablesMap& variables
			, const RoutePlannerResult& object
			, const time::Date& date
			, int periodId
			, const Place* originPlace
			, const Place* destinationPlace
			, const HourPeriod* period
			, const AccessParameters& accessParameters
			, const server::Request* request /*= NULL*/
			, const AccessibilityParameter& accessibility
			, const Site* site
			, bool samePlaces
		) const	{
			const void* vobj(static_cast<const void*>(&object));
			const City* originCity(dynamic_cast<const City*>(originPlace));
			string originPlaceName;
			if (originCity == NULL)
			{
				originCity = dynamic_cast<const NamedPlace*>(originPlace)->getCity();
				originPlaceName = dynamic_cast<const NamedPlace*>(originPlace)->getName();
			}
			const City* destinationCity(dynamic_cast<const City*>(destinationPlace));
			string destinationPlaceName;
			if (destinationCity == NULL)
			{
				destinationCity = dynamic_cast<const NamedPlace*>(destinationPlace)->getCity();
				destinationPlaceName = dynamic_cast<const NamedPlace*>(destinationPlace)->getName();
			}
			
			// Text formatted date
			const DateTimeInterfacePage* datePage(getInterface()->getPage<DateTimeInterfacePage>());
			stringstream sDate;
			datePage->display(sDate, variables, date, request);

			ParametersVector pv;
			pv.push_back(date.toSQLString(false));
			pv.push_back("0");
			pv.push_back(originCity->getName());
			pv.push_back(Conversion::ToString(accessParameters.getUserClass() == USER_HANDICAPPED));
			pv.push_back(originPlaceName);
			pv.push_back(Conversion::ToString(accessParameters.getUserClass() == USER_BIKE_IN_PT));
			pv.push_back(destinationCity->getName());
			pv.push_back("" /*Conversion::ToString(destinationPlace->getKey())*/);
			pv.push_back(destinationPlaceName);
			pv.push_back(Conversion::ToString(periodId));
			pv.push_back(sDate.str());
			pv.push_back((period == NULL) ? string() : period->getCaption());
			pv.push_back(Conversion::ToString(object.result.size()));
			pv.push_back(Conversion::ToString(accessibility));
			pv.push_back(Conversion::ToString(site->getKey()));
			pv.push_back(Conversion::ToString(samePlaces));

			InterfacePage::_display(stream, pv, variables, vobj, request);
		}

		void RoutePlannerInterfacePage::display(
			ostream& stream
			, interfaces::VariablesMap& variables
			, const time::Date& date
			, int periodId
			, bool home
			, const string& originCity
			, const string& originPlace
			, const string& destinationCity
			, const string& destinationPlace
			, const transportwebsite::HourPeriod* period 
			, const AccessParameters& accessParameters
			, const server::Request* request /*= NULL  */
			, const AccessibilityParameter& accessibility
			, const Site* site
		) const	{
			// Text formatted date
			const DateTimeInterfacePage* datePage(getInterface()->getPage<DateTimeInterfacePage>());
			stringstream sDate;
			datePage->display(sDate, variables, date, request);

			ParametersVector pv;
			pv.push_back(date.toSQLString(false));
			pv.push_back(Conversion::ToString(home));
			pv.push_back(originCity);
			pv.push_back(Conversion::ToString(accessParameters.getUserClass() == USER_HANDICAPPED));
			pv.push_back(originPlace);
			pv.push_back(Conversion::ToString(accessParameters.getUserClass() == USER_BIKE_IN_PT));
			pv.push_back(destinationCity);
			pv.push_back(string());
			pv.push_back(destinationPlace);
			pv.push_back(Conversion::ToString(periodId));
			pv.push_back(sDate.str());
			pv.push_back((period == NULL) ? string() : period->getCaption());
			pv.push_back("0");
			pv.push_back(Conversion::ToString(accessibility));
			pv.push_back(Conversion::ToString(site->getKey()));
			pv.push_back("0");

			InterfacePage::_display(stream, pv, variables, NULL, request);

		}



		RoutePlannerInterfacePage::RoutePlannerInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
