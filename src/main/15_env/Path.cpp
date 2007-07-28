
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
	using namespace geometry;

	namespace env
	{

		Path::Path ()
			: Complyer()
			, _calendar ()
		{
		}
		    

		Path::~Path ()
		{
		}



		const ServiceSet& 
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




		void Path::addService (Service* service)
		{
			if (_services.find(service) != _services.end())
				throw Exception("The service already exists.");

			std::pair<ServiceSet::iterator, bool> result = _services.insert (service);
			if (result.second == false)
			{
				throw Exception ("Service number " + Conversion::ToString (service->getServiceNumber ())
						+ " is already defined in path " + Conversion::ToString (getId ()));
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
			return _allDays || _calendar.isMarked ( date );
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




		std::vector<const Point2D*> 
		Path::getPoints (int fromEdgeIndex,
				int toEdgeIndex) const
		{
			if (toEdgeIndex == -1) toEdgeIndex = _edges.size () - 1;
			std::vector<const Point2D*> points;
		    
			for (int i=fromEdgeIndex; i<=toEdgeIndex; ++i)
			{
			points.push_back (_edges[i]->getFromVertex ());
			
			// Adds all the via points of the line stop
			const std::vector<const Point2D*>& viaPoints = _edges[i]->getViaPoints ();
			for (std::vector<const Point2D*>::const_iterator it = viaPoints.begin (); 
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
			if (!_edges.empty())
			{
				_edges.back ()->setNextInPath (edge);
			
				// Chaining departure/arrival
				AddressablePlace::ConnectionType neededConnectionTypeToStep(
					edge->getParentPath ()->isRoad ()
					? AddressablePlace::CONNECTION_TYPE_ROADROAD 
					: AddressablePlace::CONNECTION_TYPE_LINELINE
					);

				for ( std::vector<Edge*>::reverse_iterator riter = _edges.rbegin();
					( riter != _edges.rend() )
					&& (
						(( *riter )->getFollowingConnectionArrival () == 0) ||
						(( *riter )->getFollowingArrivalForFineSteppingOnly () == 0) ||
						(edge->getPreviousDepartureForFineSteppingOnly () == 0) ||
						(edge->getPreviousConnectionDeparture () == 0)
						);
					++riter
				){
					Edge* currentEdge = *riter;
		         
					/* TODO : Check later on if the following block should be re-introduced
					// Chain only relations between A and A, D and D, A and D 
					// if different stops, D and A if different stops
						if ((currentEdge->getFromVertex ()->getPlace () == 
					edge->getFromVertex ()->getPlace ()) && 
					(currentEdge->getType () != edge->getType ()) ) continue;
					*/
				    
					if (edge->isArrival())
					{
						// Chain following arrivals
						if( currentEdge->getFollowingArrivalForFineSteppingOnly () == NULL)
						{
							currentEdge->setFollowingArrivalForFineSteppingOnly (edge);
						}

						// Chain following connecting arrivals
						if(	currentEdge->getFollowingConnectionArrival () == NULL
							&& edge->getFromVertex ()->getPlace ()->getConnectionType () >= neededConnectionTypeToStep
						){
							currentEdge->setFollowingConnectionArrival (edge);
						} 
					}


					// Chain previous departures
					if (currentEdge->isDeparture ())
					{
						if (edge->getPreviousDepartureForFineSteppingOnly () == NULL)
						{   
							edge->setPreviousDepartureForFineSteppingOnly (currentEdge);
						}

						// Chain previous connecting departures
						if(	edge->getPreviousConnectionDeparture () == NULL
							&& currentEdge->getFromVertex ()->getPlace ()->getConnectionType () >= neededConnectionTypeToStep
						){
							edge->setPreviousConnectionDeparture (currentEdge);
						}
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

		void Path::updateScheduleIndexes()
		{
			for (Edges::const_iterator it = _edges.begin(); it != _edges.end(); ++it)
				(*it)->updateServiceIndex();
		}

		void Path::setAllDays( bool value )
		{
			_allDays = value;
		}
	}
}
