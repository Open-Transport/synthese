
/** Path class implementation.
	@file Path.cpp

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

#include "Path.h"
#include "ConnectionPlace.h"
#include "Edge.h"
#include "Vertex.h"
#include "Service.h"
#include "Road.h"
#include "Exception.h"

#include "01_util/Conversion.h"

#include "17_messages/Alarm.h"

#include <assert.h>

using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	namespace env
	{

		Path::Path ()
			: BikeComplyer (0) // No parent complyer right now
			, HandicappedComplyer (0) // No parent complyer right now
			, PedestrianComplyer (0) // No parent complyer right now
			, ReservationRuleComplyer (0) // No parent complyer right now
			, _calendar ()
			, _fare(NULL)
			, _alarm(NULL)
		{
		}
		    

		Path::~Path ()
		{
		}



		bool 
		Path::hasApplicableAlarm (const synthese::time::DateTime& start, 
					const synthese::time::DateTime& end) const
		{
			if (_alarm == 0) return false;
			return _alarm->isApplicable (start, end);
		}



		const Alarm* 
		Path::getAlarm() const
		{
			return _alarm;
		}



		void
		Path::setAlarm (Alarm* alarm)
		{
			_alarm = alarm;
		}



		const Fare* 
		Path::getFare () const
		{
			return _fare;
		}



		void 
		Path::setFare (Fare* fare)
		{
			_fare = fare;
		}





		const Path::ServiceSet& 
		Path::getServices () const
		{
			return _services;
		}






		const Service* 
		Path::getService (int serviceIndex) const
		{
			ServiceSet::const_iterator it(_services.begin ());
			advance (it, serviceIndex);
			return (*it);
		}




		void 
		Path::addService (Service* service,
				const std::vector<synthese::time::Schedule>& departureSchedules,
				const std::vector<synthese::time::Schedule>& arrivalSchedules)
		{
			if (_services.find(service) != _services.end())
				throw Exception("toto");

			std::pair<ServiceSet::iterator, bool> result = _services.insert (service);
			if (result.second == false)
			{
				throw Exception ("Service number " + Conversion::ToString (service->getServiceNumber ())
						+ " is already defined in path " + Conversion::ToString (getId ()));
			}
			
			// Otherwise updates each edge
			int index = distance (_services.begin (), result.first);
			for (int i=0; i<_edges.size (); ++i)
			{
				_edges[i]->insertDepartureSchedule (index, departureSchedules.at (i));
				_edges[i]->insertArrivalSchedule (index, arrivalSchedules.at (i));
			}
		}



		void 
		Path::removeService (Service* service)
		{
			// TODO 

		}




		bool 
		Path::isInService (const synthese::time::Date& date) const
		{
			return _calendar.isMarked ( date );

		}




		void 
		Path::updateCalendar ()
		{
			/* MJ TODO REVIEW this

			_calendar.reset ();
			LineStop* lastLineStop = _lineStops.back();
			for (int s=0; s<_services.size (); ++s)
			{
				if ( lastLineStop->getLastArrivalSchedule (s).getDaysSinceDeparture() != 
				_lineStops.front()->getFirstDepartureSchedule(s).getDaysSinceDeparture() )
				{
					_calendar.reset (true);
					break;
				}
			// MJ constness problem !
				((Service*) getService (s))->getCalendar ()->setInclusionToMask (_calendar);
			}
			*/

		}


		const Edge* 
		Path::getEdge (int index) const
		{
			return _edges[index];
		}




		const std::vector<Edge*>& 
		Path::getEdges() const
		{
			return _edges;
		}




		std::vector<const Point*> 
		Path::getPoints (int fromEdgeIndex,
				int toEdgeIndex) const
		{
			if (toEdgeIndex == -1) toEdgeIndex = _edges.size () - 1;
			std::vector<const Point*> points;
		    
			for (int i=fromEdgeIndex; i<=toEdgeIndex; ++i)
			{
			points.push_back (_edges[i]->getFromVertex ());
			
			// Adds all the via points of the line stop
			const std::vector<const Point*>& viaPoints = _edges[i]->getViaPoints ();
			for (std::vector<const Point*>::const_iterator it = viaPoints.begin (); 
				it != viaPoints.end (); 
				++it)
			{
				points.push_back (*it);
			}
			}
			return points;
		}



		void Path::addEdge (Edge* edge)
		{
			if (_edges.empty() == false)
			{
				_edges.back ()->setNextInPath (edge);
			
				// Chaining departure/arrival
				for ( std::vector<Edge*>::reverse_iterator riter = _edges.rbegin();
				( riter != _edges.rend() )
				&& (
					(( *riter )->getFollowingConnectionArrival () == 0) ||
					(( *riter )->getFollowingArrivalForFineSteppingOnly () == 0) ||
					(edge->getPreviousDepartureForFineSteppingOnly () == 0) ||
					(edge->getPreviousConnectionDeparture () == 0)
					);
				++riter )
				{
					Edge* currentEdge = *riter;
		         
					/* TODO : Check later on if the following block should be re-introduced
					// Chain only relations between A and A, D and D, A and D 
					// if different stops, D and A if different stops
						if ((currentEdge->getFromVertex ()->getPlace () == 
					edge->getFromVertex ()->getPlace ()) && 
					(currentEdge->getType () != edge->getType ()) ) continue;
					*/
				    
					// Chain following arrivals
					if ( currentEdge->getFollowingArrivalForFineSteppingOnly () == 0 && 
					edge->isArrival () )
					{
						currentEdge->setFollowingArrivalForFineSteppingOnly (edge);
					}

					// Chain following connecting arrivals
					if ( (currentEdge->getFollowingConnectionArrival () == 0) && 
					edge->getFromVertex ()->getPlace ()->getConnectionType () >= 
					(currentEdge->getParentPath ()->isRoad ()
					? AddressablePlace::CONNECTION_TYPE_ROADROAD 
					: AddressablePlace::CONNECTION_TYPE_LINELINE ) )
					{
						currentEdge->setFollowingConnectionArrival (edge);
					} 


					// Chain previous departures
					if ( currentEdge->isDeparture () && 
					edge->getPreviousDepartureForFineSteppingOnly () == 0 )
					{   
						edge->setPreviousDepartureForFineSteppingOnly (currentEdge);
					}


					// Chain previous connecting departures
					if ( currentEdge->isDeparture () && 
					(edge->getPreviousConnectionDeparture () == 0) && 
					currentEdge->getFromVertex ()->getPlace ()->getConnectionType () >= 
					(edge->getParentPath ()->isRoad () 
					? AddressablePlace::CONNECTION_TYPE_ROADROAD 
					: AddressablePlace::CONNECTION_TYPE_LINELINE ) )
					{
						edge->setPreviousConnectionDeparture (currentEdge);
					}
				}

			}

			_edges.push_back( edge );

			/* TODO : Check later on if the following block should be re-introduced (see above)
			* Normaly should be completely useless given that all the D,P,A chaining is provided from
			* db or from XML.

			for ( std::vector<Edge*>::const_iterator iter = _edges.begin();
			iter != _edges.end();
			++iter )
			{
				Edge* edge = *iter;

				if ( edge->getFollowingArrival () == 0 )
					edge->setType ( Edge::EDGE_TYPE_ARRIVAL );
				if ( edge->getPreviousDeparture () == 0 )
					edge->setType ( Edge::EDGE_TYPE_DEPARTURE );
			}
			*/		    
		}



		int 
		Path::getEdgesCount () const
		{
			return _edges.size ();
		}

		Edge* Path::getLastEdge() const
		{
			vector<Edge*>::const_iterator it = _edges.end();
			--it;
			return (it != _edges.end()) ? *it : NULL;
		}

		Calendar& Path::getCalendar()
		{
			return _calendar;
		}

	}
}
