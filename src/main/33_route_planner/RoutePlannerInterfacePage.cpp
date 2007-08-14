
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

#include "36_places_list/HourPeriod.h"

#include "15_env/City.h"
#include "15_env/Place.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/BikeCompliance.h"

#include "11_interfaces/DateTimeInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "04_time/Date.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace util;
	using namespace transportwebsite;

	namespace routeplanner
	{

		void RoutePlannerInterfacePage::display(
			std::ostream& stream
			, VariablesMap& variables
			, const JourneyBoardJourneys& object /*= NULL*/
			, const time::Date& date
			, int periodId
			, const env::Place* originPlace
			, const env::Place* destinationPlace
			, const HourPeriod* period
			, const AccessParameters& accessParameters
			, const server::Request* request /*= NULL*/
			) const
		{
			const void* vobj(static_cast<const void*>(&object));
			const City* originCity(dynamic_cast<const City*>(originPlace));
			string originPlaceName;
			if (originCity == NULL)
			{
				originCity = originPlace->getCity();
				originPlaceName = originPlace->getName();
			}
			const City* destinationCity(dynamic_cast<const City*>(destinationPlace));
			string destinationPlaceName;
			if (destinationCity == NULL)
			{
				destinationCity = destinationPlace->getCity();
				destinationPlaceName = destinationPlace->getName();
			}
			
			// Text formatted date
			shared_ptr<const DateTimeInterfacePage> datePage(getInterface()->getPage<DateTimeInterfacePage>());
			stringstream sDate;
			datePage->display(sDate, variables, date, request);

			ParametersVector pv;
			pv.push_back(date.toInternalString());
			pv.push_back("0");
			pv.push_back(originCity->getName());
			pv.push_back(Conversion::ToString(accessParameters.complyer.getHandicappedCompliance()->isCompliant()));
			pv.push_back(originPlaceName);
			pv.push_back(Conversion::ToString(accessParameters.complyer.getBikeCompliance()->isCompliant()));
			pv.push_back(destinationCity->getName());
			pv.push_back(Conversion::ToString(destinationPlace->getId()));
			pv.push_back(destinationPlaceName);
			pv.push_back(Conversion::ToString(periodId));
			pv.push_back(sDate.str());
			pv.push_back((period == NULL) ? string() : period->getCaption());
			pv.push_back(Conversion::ToString(object.size()));

			InterfacePage::display(stream, pv, variables, vobj, request);
		}

		void RoutePlannerInterfacePage::display( std::ostream& stream , interfaces::VariablesMap& variables , const time::Date& date , int periodId , bool home, const std::string& originCity , const std::string& originPlace , const std::string& destinationCity , const std::string& destinationPlace , const transportwebsite::HourPeriod* period 
			, const AccessParameters& accessParameters
			, const server::Request* request /*= NULL  */
		) const	{
			// Text formatted date
			shared_ptr<const DateTimeInterfacePage> datePage(getInterface()->getPage<DateTimeInterfacePage>());
			stringstream sDate;
			datePage->display(sDate, variables, date, request);

			ParametersVector pv;
			pv.push_back(date.toInternalString());
			pv.push_back(Conversion::ToString(home));
			pv.push_back(originCity);
			pv.push_back(Conversion::ToString(accessParameters.complyer.getHandicappedCompliance()->isCompliant()));
			pv.push_back(originPlace);
			pv.push_back(Conversion::ToString(accessParameters.complyer.getBikeCompliance()->isCompliant()));
			pv.push_back(destinationCity);
			pv.push_back(string());
			pv.push_back(destinationPlace);
			pv.push_back(Conversion::ToString(periodId));
			pv.push_back(sDate.str());
			pv.push_back((period == NULL) ? string() : period->getCaption());
			pv.push_back("0");

			InterfacePage::display(stream, pv, variables, NULL, request);

		}
	}
}
