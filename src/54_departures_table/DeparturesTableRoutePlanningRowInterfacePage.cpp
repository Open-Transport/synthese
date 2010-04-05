////////////////////////////////////////////////////////////////////////////////
/// DepartureTableRowInterfacePage class implementation.
///	@file DepartureTableRowInterfacePage.cpp
///	@author Hugues Romain
//
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DeparturesTableRoutePlanningRowInterfacePage.h"
#include <boost/lexical_cast.hpp>
#include "Journey.h"
#include "ServiceUse.h"
#include "PhysicalStop.h"
#include "Service.h"
#include "Line.h"
#include "CommercialLine.h"
#include "Edge.h"
#include "City.h"
#include "RollingStock.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;


namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace graph;
	using namespace pt;
	using namespace pt;
	

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::InterfacePage, departurestable::DeparturesTableRoutePlanningRowInterfacePage>::FACTORY_KEY("departurestablerouteplanningrow");
	}

	namespace departurestable
	{
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_RANK("rank");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_WITH_TRANSFER("with_transfer");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DISPLAY_TRACK("display_track");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DISPLAY_SERVICE_NUMBER("display_service_number");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_TRACK("first_track");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_SERVICE("first_service_number");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_TIME("first_departure_time");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_LINE_TEXT("first_line_text");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_LINE_IMAGE("first_line_image");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_FIRST_LINE_STYLE("first_line_style");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_TRACK("second_track");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_SERVICE("second_service_number");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_TIME("second_departure_time");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_LINE_TEXT("second_line_text");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_LINE_IMAGE("second_line_image");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_SECOND_LINE_STYLE("second_line_style");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DESTINATION_CITY_NAME("destination_city_name");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DESTINATION_CITY_IS_DIFFERENT("destination_city_is_different");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DESTINATION_PLACE_NAME("destination_place_name");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DESTINATION_PLACE_NAME13("destination_place_name13");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_DESTINATION_PLACE_NAME26("destination_place_name26");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_TRANSFER_CITY_NAME("transfer_city_name");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_TRANSFER_CITY_IS_DIFFERENT("transfer_city_is_different");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_TRANSFER_PLACE_NAME("transfer_place_name");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_TRANSFER_PLACE_NAME13("transfer_place_name13");
		const string DeparturesTableRoutePlanningRowInterfacePage::DATA_TRANSFER_PLACE_NAME26("transfer_place_name26");

		DeparturesTableRoutePlanningRowInterfacePage::DeparturesTableRoutePlanningRowInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}

		void DeparturesTableRoutePlanningRowInterfacePage::display(
			std::ostream& stream ,
			interfaces::VariablesMap& vars ,
			int rowId ,
			bool displayQuaiNumber ,
			bool displayServiceNumber ,
			int blinkingDelay,
			bool withTransfer,
			const pt::PublicTransportStopZoneConnectionPlace& origin,
			const RoutePlanningRow& row ,
			const server::Request* request /*= NULL */
		) const	{

			ParametersVector v;

			// 0
			v.push_back(lexical_cast<string>(rowId));
			v.push_back(lexical_cast<string>(withTransfer));
			v.push_back(lexical_cast<string>(displayQuaiNumber));
			v.push_back(lexical_cast<string>(displayServiceNumber));

			// 5
			if(!row.second.empty())
			{
				const ServiceUse& s(row.second.getStartServiceUse());

				v.push_back(lexical_cast<string>(s.getDepartureDateTime() - second_clock::local_time() <= posix_time::minutes(blinkingDelay)));
				v.push_back(static_cast<const PhysicalStop*>(s.getDepartureRTVertex())->getName());
				v.push_back(s.getService()->getServiceNumber());
				stringstream str;
				str << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().minutes();
				v.push_back(str.str());
				const CommercialLine* line(static_cast<const Line*>(s.getEdge()->getParentPath())->getCommercialLine());
				v.push_back(line->getShortName());
				v.push_back(line->getImage());
				v.push_back(line->getStyle());

				if(row.second.getServiceUses().size() > 1)
				{
					const ServiceUse& s(row.second.getEndServiceUse());
					v.push_back(static_cast<const PhysicalStop*>(s.getDepartureRTVertex())->getName());
					v.push_back(s.getService()->getServiceNumber());

					stringstream str;
					str << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().minutes();
					v.push_back(str.str());

					const CommercialLine* line(static_cast<const Line*>(s.getEdge()->getParentPath())->getCommercialLine());
					v.push_back(line->getShortName());
					v.push_back(line->getImage());
					v.push_back(line->getStyle());
				}
				else
				{
					v.push_back(string());
					v.push_back(string());
					v.push_back(string());
					v.push_back(string());
					v.push_back(string());
					v.push_back(string());
				}
			}
			else
			{
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
			}

			//17
			v.push_back(row.first->getCity()->getName());
			v.push_back(lexical_cast<string>(row.first->getCity()->getKey() != origin.getCity()->getKey()));
			v.push_back(row.first->getName());
			v.push_back(row.first->getName13());
			v.push_back(row.first->getName26());

			//22
			if(row.second.getServiceUses().size() > 1)
			{
				const ServiceUse& s(row.second.getEndServiceUse());
				const PublicTransportStopZoneConnectionPlace* p(static_cast<const PublicTransportStopZoneConnectionPlace*>(s.getDepartureEdge()->getFromVertex()->getHub()));
				v.push_back(p->getCity()->getName());
				v.push_back(lexical_cast<string>(p->getCity()->getKey() != origin.getCity()->getKey()));
				v.push_back(p->getName());
				v.push_back(p->getName13());
				v.push_back(p->getName26());
			}
			else
			{
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
				v.push_back(string());
			}

			if(!row.second.empty())
			{
				const ServiceUse& s(row.second.getStartServiceUse());

				const Line* line(static_cast<const Line*>(s.getEdge()->getParentPath()));
				v.push_back(line->getRollingStock() ? lexical_cast<string>(line->getRollingStock()->getKey()) : string());

				if(row.second.getServiceUses().size() > 1)
				{
					const ServiceUse& s(row.second.getEndServiceUse());
					const Line* line(static_cast<const Line*>(s.getEdge()->getParentPath()));
					v.push_back(line->getRollingStock() ? lexical_cast<string>(line->getRollingStock()->getKey()) : string());
				}
				else
				{
					v.push_back(string());
				}
			}
			else
			{
				v.push_back(string());
				v.push_back(string());
			}

			_display(stream, v, vars, (void*) &row, request);

		}
	}
}
