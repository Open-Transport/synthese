
//////////////////////////////////////////////////////////////////////////////////////////
///	TransferLinesService class implementation.
///	@file TransferLinesService.cpp
///	@author User
///	@date 2012
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

#include "TransferLinesService.hpp"

#include "CommercialLine.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "RequestException.h"
#include "Request.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace util;
	using namespace pt;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,departure_boards::TransferLinesService>::FACTORY_KEY = "transfer_lines";
	
	namespace departure_boards
	{
		const string TransferLinesService::PARAMETER_LINE_ID = "line_id";
		const string TransferLinesService::PARAMETER_STOP_AREA_ID = "stop_area_id";
		const string TransferLinesService::PARAMETER_TIME = "time";
		const string TransferLinesService::PARAMETER_DURATION = "duration";
		
		const string TransferLinesService::TAG_LINE = "line";



		ParametersMap TransferLinesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void TransferLinesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Line
			try
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(
					map.get<RegistryKeyType>(PARAMETER_LINE_ID)
				);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such line");
			}

			// Stop area
			try
			{
				_stopArea = Env::GetOfficialEnv().get<StopArea>(
					map.get<RegistryKeyType>(PARAMETER_STOP_AREA_ID)
				);
			}
			catch(ObjectNotFoundException<StopArea>&)
			{
				throw RequestException("No such stop area");
			}

			// Time
			if(map.getOptional<string>(PARAMETER_TIME))
			{
				_time = time_from_string(map.get<string>(PARAMETER_TIME));
			}
			else
			{
				_time = second_clock::local_time();
			}

			// Duration
			_duration = duration_from_string(map.getDefault<string>(PARAMETER_DURATION, "02:00"));
		}



		ParametersMap TransferLinesService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Populating a list of nearby stops
			typedef std::map<const StopArea*, LinesSet> LinesSetMap;
			LinesSetMap nearbyStops;
			BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, _stopArea->getPhysicalStops())
			{
				// Loop on departures
				BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, itStop.second->getDepartureEdges())
				{
					// Jump over routes from other lines
					if(	!dynamic_cast<const JourneyPattern*>(itEdge.first) ||
						static_cast<const JourneyPattern*>(itEdge.first)->getCommercialLine() !=
						_line.get()
					){
						continue;
					}

					// Store the stop area
					nearbyStops.insert(
						make_pair(
							static_cast<const StopArea*>(itEdge.second->getNext()->getFromVertex()->getHub()),
							LinesSet()
					)	);
				}
			}

			// Getting the list of the lines calling at each stop
			LinesSet linesAtCurrentStop(
				_getLinesCallingAtStop(*_stopArea)
			);
			BOOST_FOREACH(LinesSetMap::value_type& itLSM, nearbyStops)
			{
				itLSM.second = _getLinesCallingAtStop(*itLSM.first);
			}

			// Testing if line will be included in the result
			LinesSet result;
			BOOST_FOREACH(const LinesSet::value_type& line, linesAtCurrentStop)
			{
				// Check if the line is not in one of the other lines lists
				bool toBeIncluded(false);
				BOOST_FOREACH(LinesSetMap::value_type& itLSM, nearbyStops)
				{
					if(itLSM.second.find(line) == itLSM.second.end())
					{
						toBeIncluded = true;
						break;
					}
				}

				// Jump over useless lines
				if(!toBeIncluded)
				{
					continue;
				}

				// Line registration
				result.insert(line);
			}

			// Remove the current line from the result (case of unused deviated route)
			result.erase(_line.get());

			// Export of the result in the parameters map
			ParametersMap map;
			BOOST_FOREACH(const LinesSet::value_type& line, result)
			{
				boost::shared_ptr<ParametersMap> lineMap(new ParametersMap);
				line->toParametersMap(*lineMap);
				map.insert(TAG_LINE, lineMap);
			}
			return map;
		}
		
		
		
		bool TransferLinesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string TransferLinesService::getOutputMimeType() const
		{
			return "text/html";
		}



		TransferLinesService::LinesSet TransferLinesService::_getLinesCallingAtStop(
			const pt::StopArea& stopArea
		) const	{

			LinesSet result;

			ArrivalDepartureTableGenerator::PhysicalStops ps(stopArea.getPhysicalStops());
			DeparturesTableDirection di(DISPLAY_DEPARTURES);
			EndFilter ef(WITH_PASSING);
			LineFilter lf;
			DisplayedPlacesList dp;
			ForbiddenPlacesList fp;
			ptime endTime(_time + _duration);
			StandardArrivalDepartureTableGenerator tdg(
				ps,
				di,
				ef,
				lf,
				dp,
				fp,
				_time,
				endTime,
				false
			);
			BOOST_FOREACH(const ArrivalDepartureList::value_type& itService, tdg.generate())
			{
				result.insert(
					static_cast<const JourneyPattern*>(itService.first.getDepartureEdge()->getParentPath())->getCommercialLine()
				);
			}

			return result;
		}
}	}
