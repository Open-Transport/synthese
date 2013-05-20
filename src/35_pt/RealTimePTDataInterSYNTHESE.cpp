
/** RealTimePTDataInterSYNTHESE class implementation.
	@file RealTimePTDataInterSYNTHESE.cpp

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

#include "RealTimePTDataInterSYNTHESE.hpp"

#include "CommercialLine.h"
#include "ContinuousServiceTableSync.h"
#include "InterSYNTHESEIdFilter.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopPoint.hpp"
#include "TransportNetwork.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace inter_synthese;
	using namespace util;
	
	
	template<>
	const string FactorableTemplate<InterSYNTHESESyncTypeFactory, pt::RealTimePTDataInterSYNTHESE>::FACTORY_KEY = "ptrt";



	namespace pt
	{
		const std::string RealTimePTDataInterSYNTHESE::FIELD_SEPARATOR = ",";



		RealTimePTDataInterSYNTHESE::RealTimePTDataInterSYNTHESE():
			FactorableTemplate<inter_synthese::InterSYNTHESESyncTypeFactory, RealTimePTDataInterSYNTHESE>()
		{
		}



		/// Config perimeter = network id
		/// Message perimeter = line id
		bool RealTimePTDataInterSYNTHESE::mustBeEnqueued(
			const std::string& configPerimeter,
			const std::string& messagePerimeter
		) const	{

			RegistryKeyType lineId(lexical_cast<RegistryKeyType>(messagePerimeter));
			boost::shared_ptr<const CommercialLine> line(Env::GetOfficialEnv().get<CommercialLine>(lineId));
			RegistryKeyType networkId(lexical_cast<RegistryKeyType>(configPerimeter));
			return line->getNetwork()->getKey() == networkId;

		}



		void RealTimePTDataInterSYNTHESE::initSync() const
		{

		}



		bool RealTimePTDataInterSYNTHESE::sync(
			const string& parameter,
			const InterSYNTHESEIdFilter* idFilter
		) const	{

			if(parameter.empty())
			{
				return false;
			}

			vector<string> fields;
			split(fields, parameter, is_any_of(FIELD_SEPARATOR));

			if(fields.size() < 4)
			{
				return false;
			}

			// Load of the service
			boost::shared_ptr<SchedulesBasedService> service;
			try
			{
				RegistryKeyType serviceId(
					lexical_cast<RegistryKeyType>(fields[0])
				);
				if(decodeTableId(serviceId) == ScheduledServiceTableSync::TABLE.ID)
				{
					service = Env::GetOfficialEnv().getCastEditable<SchedulesBasedService, ScheduledService>(serviceId);
				}
				else if(decodeTableId(serviceId) == ContinuousServiceTableSync::TABLE.ID)
				{
					service = Env::GetOfficialEnv().getCastEditable<SchedulesBasedService, ContinuousService>(serviceId);
				}
			}
			catch (bad_lexical_cast&)
			{
				return false;
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				return false;
			}

			SchedulesBasedService::Schedules departureSchedules(service->getDepartureSchedules(true, true));
			SchedulesBasedService::Schedules arrivalSchedules(service->getArrivalSchedules(true, true));
			SchedulesBasedService::ServedVertices vertices(service->getVertices(true));
			for(size_t i(1); i+2<fields.size(); i+=4)
			{
				try
				{
					// Stop rank
					size_t rank(lexical_cast<size_t>(fields[i]));

					// Arrival schedule
					time_duration arr(duration_from_string(fields[i+1]));

					// Departure schedule
					time_duration dep(duration_from_string(fields[i+2]));

					// Served vertex
					Vertex* vertex(NULL);
					if(	i+3 < fields.size() &&
						!fields[i+3].empty()
					){
						string vertexIdStr = idFilter->convertId(0, string(), fields[i+3]);
						vertex = Env::GetOfficialEnv().getEditable<StopPoint>(
							lexical_cast<RegistryKeyType>(vertexIdStr)
						).get();
					}

					// Saving
					departureSchedules[rank] = dep;
					arrivalSchedules[rank] = arr;
					vertices[rank] = vertex;
				}
				catch(...)
				{
				}
			}

			// The update
			service->setRealTimeVertices(
				vertices
			);
			service->setRealTimeSchedules(
				departureSchedules,
				arrivalSchedules
			);

			return true;
		}



		void RealTimePTDataInterSYNTHESE::closeSync() const
		{

		}



		void RealTimePTDataInterSYNTHESE::initQueue(
			const inter_synthese::InterSYNTHESESlave& slave,
			const std::string& perimeter
		) const	{

		}



		string RealTimePTDataInterSYNTHESE::GetContent(
			const SchedulesBasedService& service,
			optional<const RanksToSync&> ranksToSync
		){
			stringstream result;
			result << service.getKey();
			size_t maxRank(service.getRoute()->getEdges().size());
			for(size_t i(0); i<maxRank; ++i)
			{
				if(!ranksToSync || ranksToSync->at(i))
				{
					result <<
						FIELD_SEPARATOR <<
						i <<
						FIELD_SEPARATOR <<
						to_simple_string(service.getArrivalSchedule(true, i)) <<
						FIELD_SEPARATOR <<
						to_simple_string(service.getDepartureSchedule(true, i)) <<
						FIELD_SEPARATOR
					;
					if(service.getRealTimeVertex(i))
					{
						result << service.getRealTimeVertex(i)->getKey();
					}
				}
			}
			return result.str();
		}
}	}
