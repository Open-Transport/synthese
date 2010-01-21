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

#include "Conversion.h"
#include "DepartureTableRowInterfacePage.h"
#include "DateTime.h"
#include "ScheduledService.h"
#include "ServicePointer.h"
#include "PhysicalStop.h"
#include "Edge.h"
#include "Line.h"
#include "RollingStock.h"
#include "RealTimeUpdateScreenServiceInterfacePage.h"
#include "RealTimeUpdateFunction.h"
#include "Interface.h"
#include "StaticFunctionRequest.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace time;
	using namespace env;
	using namespace pt;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::InterfacePage, departurestable::DepartureTableRowInterfacePage>::FACTORY_KEY("departurestablerow");
	}

	namespace departurestable
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
			parameters.push_back(Conversion::ToString(rowId)); //0
			parameters.push_back(Conversion::ToString(pageNumber));
			parameters.push_back(Conversion::ToString(displayQuai));
			parameters.push_back(Conversion::ToString(displayServiceNumber));
			parameters.push_back(Conversion::ToString(intermediatesStopsToDisplay));
			parameters.push_back(Conversion::ToString(displayTeam)); //5
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
					(blinkingDelay > 0 && ptd.first.getActualDateTime().getSecondsDifference(DateTime(TIME_CURRENT)) <= posix_time::minutes(blinkingDelay)) ?
					string("1") : string("0")
					);
				parameters.push_back(ptd.first.getActualDateTime().getHour().toString());
				parameters.push_back(ptd.first.getService()->getServiceNumber());
				parameters.push_back(
					static_cast<const PhysicalStop*>(ptd.first.getRealTimeVertex())->getName()
					);
				parameters.push_back(
					ptd.first.getService()->getTeam()
				);
				const Line* line(static_cast<const Line*>(ptd.first.getEdge()->getParentPath()));
				parameters.push_back(line->getRollingStock() ? lexical_cast<string>(line->getRollingStock()->getKey()) : string());

				parameters.push_back(
					ptd.first.getTheoreticalDateTime().getHour().toString()
				);

				parameters.push_back(
					lexical_cast<string>(ptd.first.getActualDateTime().getSecondsDifference(ptd.first.getTheoreticalDateTime()).total_seconds() / 60)
				); //13

				if(	getInterface()->hasPage<RealTimeUpdateScreenServiceInterfacePage>() &&
					dynamic_cast<const ScheduledService*>(ptd.first.getService()) &&
					request
				){
					StaticFunctionRequest<RealTimeUpdateFunction> realTimeRequest(*request, true);
					realTimeRequest.getFunction()->setInterface(Env::GetOfficialEnv().getRegistry<Interface>().get(getInterface()->getKey()));
					realTimeRequest.getFunction()->setService(Env::GetOfficialEnv().getRegistry<ScheduledService>().get(ptd.first.getService()->getKey()));
					realTimeRequest.getFunction()->setLineStopRank(ptd.first.getEdge()->getRankInPath());
					parameters.push_back(realTimeRequest.getURL());				
				}
				else
				{
					parameters.push_back(string());
				} //14
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
		):	Registrable(UNKNOWN_VALUE)
		{
		}
	}
}
