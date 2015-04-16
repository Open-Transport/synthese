
/** JourneyTemplates class implementation.
	@file JourneyTemplates.cpp

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

#include "JourneyTemplates.h"
#include "RoutePlanningIntermediateJourney.hpp"
#include "Edge.h"
#include "Vertex.h"
#include "ServicePointer.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		void JourneyTemplates::addResult( const Journey& value )
		{
			Hubs::value_type result;
			BOOST_FOREACH(const Journey::ServiceUses::value_type& service, value.getServiceUses())
			{
				if(service.getDepartureEdge()->getFromVertex()->getGraphType() == _graphToUse)
				{
					result.push_back(service.getDepartureEdge()->getFromVertex()->getHub());
				}
				if(service.getArrivalEdge()->getFromVertex()->getGraphType() == _graphToUse)
				{
					result.push_back(service.getArrivalEdge()->getFromVertex()->getHub());
				}
			}
			if(_hubs.find(result) == _hubs.end())
			{
				_hubs.insert(result);
			}
		}



		JourneyTemplates::JourneyTemplates( const graph::GraphIdType graphToUse ):
			_graphToUse(graphToUse)
		{

		}


/*
		bool JourneyTemplates::testJourneySimilarity( const graph::Journey& value ) const
		{
			BOOST_FOREACH(const Hubs::value_type& journeyTemplate, _hubs)
			{
				if(2 * value.size() > journeyTemplate.size())
				{
					continue;
				}

				Hubs::value_type::const_iterator it(journeyTemplate.begin());
				bool ok(true);
				BOOST_FOREACH(const Journey::ServiceUses::value_type& service, value.getServiceUses())
				{
					if(service.getDepartureEdge()->getFromVertex()->getGraphType() == _graphToUse)
					{
						if(*it != service.getDepartureEdge()->getFromVertex()->getHub())
						{
							ok = false;
							break;
						}
						++it;
					}
					if(service.getArrivalEdge()->getFromVertex()->getGraphType() == _graphToUse)
					{
						if(*it != service.getArrivalEdge()->getFromVertex()->getHub())
						{
							ok = false;
							break;
						}
						++it;
					}
				}
				if(ok)
				{
					return true;
				}
			}
			return false;
		}
*/


		bool JourneyTemplates::testSimilarity(
			const RoutePlanningIntermediateJourney& precedingJourney,
			const graph::Hub& hub,
			PlanningPhase direction
		) const	{
			if(!precedingJourney.getSimilarity())
			{
				return false;
			}
			BOOST_FOREACH(const Hubs::value_type& journeyTemplate, _hubs)
			{
				if(2 * (precedingJourney.size() + 1) > journeyTemplate.size())
				{
					continue;
				}

				return journeyTemplate[
					direction == DEPARTURE_TO_ARRIVAL ?
					2 * precedingJourney.size() + 1 :
					journeyTemplate.size() - 1 - 2 * precedingJourney.size()
				] == &hub;
			}
			return false;
		}
	}
}
