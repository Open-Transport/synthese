////////////////////////////////////////////////////////////////////////////////
/// DepartureTableRowInterfacePage class implementation.
///	@file DepartureTableRowInterfacePage.cpp
///	@author Hugues Romain
//
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DepartureTableRowInterfacePage.h"
#include "ScheduledService.h"
#include "ServicePointer.h"
#include "StopPoint.hpp"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "RollingStock.hpp"
#include "RealTimeUpdateFunction.h"
#include "Interface.h"
#include "StaticFunctionRequest.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace pt;
	using namespace pt;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::InterfacePage, departure_boards::DepartureTableRowInterfacePage>::FACTORY_KEY("departurestablerow");
	}

	namespace departure_boards
	{
		const string DepartureTableRowInterfacePage::DATA_DISPLAY_SERVICE_NUMBER("display_service_number");
		const string DepartureTableRowInterfacePage::DATA_DISPLAY_TEAM("display_team");
		const string DepartureTableRowInterfacePage::DATA_DISPLAY_TRACK_NUMBER("display_track");
		const string DepartureTableRowInterfacePage::DATA_INTERMEDIATE_STOPS_NUMBER("intermediale_stops_number");
		const string DepartureTableRowInterfacePage::DATA_PAGE_NUMBER("page_number");
		const string DepartureTableRowInterfacePage::DATA_ROW_RANK("rank");
		const string DepartureTableRowInterfacePage::DATA_BLINKS("blinks");
		const string DepartureTableRowInterfacePage::DATA_TIME("time");

		void DepartureTableRowInterfacePage::display(std::ostream& stream
			, VariablesMap& vars
			, int rowId
			, int pageNumber
            , bool displayQuai
			, bool displayServiceNumber
			, bool displayTeam
			, int intermediatesStopsToDisplay,
			int blinkingDelay
			, const ArrivalDepartureRow& ptd
			, const server::Request* request
		) const {
			ParametersVector parameters;
			parameters.push_back(lexical_cast<string>(rowId)); //0
			parameters.push_back(lexical_cast<string>(pageNumber));
			parameters.push_back(lexical_cast<string>(displayQuai));
			parameters.push_back(lexical_cast<string>(displayServiceNumber));
			parameters.push_back(lexical_cast<string>(intermediatesStopsToDisplay));
			parameters.push_back(lexical_cast<string>(displayTeam)); //5
			if(ptd.first.getService() == NULL)
			{
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string());
				parameters.push_back(string()); //14
			}
			else
			{
				parameters.push_back(
					(blinkingDelay > 0 && (ptd.first.getDepartureDateTime() - second_clock::local_time()) <= posix_time::minutes(blinkingDelay)) ?
					string("1") : string("0")
				);

				{
					stringstream str;
					str << setw(2) << setfill('0') << ptd.first.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << ptd.first.getDepartureDateTime().time_of_day().minutes();
					parameters.push_back(str.str());
				}

				parameters.push_back(ptd.first.getService()->getServiceNumber());
				parameters.push_back(
					static_cast<const StopPoint*>(ptd.first.getRealTimeDepartureVertex())->getName()
					);
				parameters.push_back(
					ptd.first.getService()->getTeam()
				);
				const JourneyPattern* line(static_cast<const JourneyPattern*>(ptd.first.getService()->getPath()));
				parameters.push_back(line->getRollingStock() ? lexical_cast<string>(line->getRollingStock()->getKey()) : string());

				{
					stringstream str;
					str << setw(2) << setfill('0') << ptd.first.getTheoreticalDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << ptd.first.getTheoreticalDepartureDateTime().time_of_day().minutes();
					parameters.push_back(str.str());
				}

				parameters.push_back(
					lexical_cast<string>((ptd.first.getDepartureDateTime() - ptd.first.getTheoreticalDepartureDateTime()).total_seconds() / 60)
				); //13

				parameters.push_back(string()); //14
			}

			InterfacePage::_display(
				stream,
				parameters,
				vars,
				static_cast<const void*>(&ptd),
				request
			);
		}



		DepartureTableRowInterfacePage::DepartureTableRowInterfacePage(
		):	Registrable(0)
		{
		}
	}
}
