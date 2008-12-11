
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

#include "15_env/ConnectionPlace.h"
#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/Service.h"
#include "15_env/Road.h"
#include "15_env/Exception.h"

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
			, _allDays(false)
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




		void Path::addService(
			Service* service,
			bool ensureLineTheory
		){
			if (_services.find(service) != _services.end())
				throw Exception("The service already exists.");

			std::pair<ServiceSet::iterator, bool> result = _services.insert (service);
			if (result.second == false)
			{
				throw Exception ("Service number " + Conversion::ToString (service->getServiceNumber ())
						+ " is already defined in path " + Conversion::ToString (getKey()));
			}
			markScheduleIndexesUpdateNeeded ();
			service->setPath(this);
		}



		void 
		Path::removeService (Service* service)
		{
			// TODO 


			markScheduleIndexesUpdateNeeded ();

		}




		bool Path::isInService (const synthese::time::Date& date) const
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
			// Empty path : just put the edge in the vector

			if (_edges.empty())
			{
				_edges.push_back(edge);
				return;
			}

			// Non empty path : determinate the good position of the edge
			Edges::iterator insertionPosition;
			for(insertionPosition = _edges.begin();
				insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() <= edge->getRankInPath();
				++insertionPosition);

			// Next in path field
			if (insertionPosition != _edges.begin())
				(*(insertionPosition - 1))->setNextInPath(edge);
			if (insertionPosition != _edges.end())
				edge->setNextInPath(*insertionPosition);

			// Chaining departure/arrival
			ConnectionPlace::ConnectionType neededConnectionTypeToStep(
				edge->getParentPath ()->isRoad ()
				? ConnectionPlace::CONNECTION_TYPE_ROADROAD 
				: ConnectionPlace::CONNECTION_TYPE_LINELINE
			);

			// Connection Links before the new edge
			const Edge* oldNormalEdge((insertionPosition == _edges.begin()) ? NULL : (*(insertionPosition-1))->getFollowingArrivalForFineSteppingOnly());
			const Edge* oldConnectionEdge((insertionPosition == _edges.begin()) ? NULL : (*(insertionPosition-1))->getFollowingConnectionArrival());
			for(Edges::reverse_iterator it(insertionPosition); 
				it != _edges.rend()
				&& ((*it)->getFollowingConnectionArrival () == oldConnectionEdge
					|| (*it)->getFollowingArrivalForFineSteppingOnly () == oldNormalEdge
					|| edge->getPreviousDepartureForFineSteppingOnly () == NULL
					|| edge->getPreviousConnectionDeparture () == NULL
				);
				++it
			){
				Edge* currentEdge = *it;

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
					if( currentEdge->getFollowingArrivalForFineSteppingOnly () == oldNormalEdge)
					{
						currentEdge->setFollowingArrivalForFineSteppingOnly (edge);
					}

					// Chain following connecting arrivals
					if(	currentEdge->getFollowingConnectionArrival () == oldConnectionEdge
						&& edge->getConnectionPlace()
						&& edge->getConnectionPlace()->getConnectionType () >= neededConnectionTypeToStep
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
						&& currentEdge->getConnectionPlace()
						&& currentEdge->getConnectionPlace()->getConnectionType () >= neededConnectionTypeToStep
					){
							edge->setPreviousConnectionDeparture (currentEdge);
					}
				}
			}
		
			// Connection Links after the new edge
			oldNormalEdge = (insertionPosition == _edges.end()) ? NULL : (*insertionPosition)->getPreviousDepartureForFineSteppingOnly();
			oldConnectionEdge = (insertionPosition == _edges.end()) ? NULL : (*insertionPosition)->getPreviousConnectionDeparture();
			for(Edges::iterator it(insertionPosition); 
				it != _edges.end()
				&& ((*it)->getPreviousConnectionDeparture() == oldConnectionEdge
					|| (*it)->getPreviousDepartureForFineSteppingOnly () == oldNormalEdge
					|| edge->getFollowingConnectionArrival() == NULL
					|| edge->getFollowingArrivalForFineSteppingOnly() == NULL
				);
				++it
			){
				Edge* currentEdge = *it;

				/* TODO : Check later on if the following block should be re-introduced
				// Chain only relations between A and A, D and D, A and D 
				// if different stops, D and A if different stops
				if ((currentEdge->getFromVertex ()->getPlace () == 
				edge->getFromVertex ()->getPlace ()) && 
				(currentEdge->getType () != edge->getType ()) ) continue;
				*/

				if (edge->isDeparture())
				{
					// Chain following arrivals
					if( currentEdge->getPreviousDepartureForFineSteppingOnly () == oldNormalEdge)
					{
						currentEdge->setPreviousDepartureForFineSteppingOnly (edge);
					}

					// Chain following connecting arrivals
					if(	currentEdge->getPreviousConnectionDeparture() == oldConnectionEdge
						&& edge->getConnectionPlace()
						&& edge->getConnectionPlace()->getConnectionType () >= neededConnectionTypeToStep
					){
							currentEdge->setPreviousConnectionDeparture(edge);
					} 
				}


				// Chain previous departures
				if (currentEdge->isArrival())
				{
					if (edge->getFollowingArrivalForFineSteppingOnly() == NULL)
					{   
						edge->setFollowingArrivalForFineSteppingOnly(currentEdge);
					}

					// Chain previous connecting departures
					if(	edge->getFollowingConnectionArrival() == NULL
						&& currentEdge->getConnectionPlace()
						&& currentEdge->getConnectionPlace()->getConnectionType () >= neededConnectionTypeToStep
					){
							edge->setFollowingConnectionArrival(currentEdge);
					}
				}
			}
		
			// Insertion of the new edges
			_edges.insert(insertionPosition, edge);
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



		const Calendar& Path::getCalendar() const
		{
			return _calendar;
		}


	    void 
	    Path::markScheduleIndexesUpdateNeeded()
	    {
		for (Edges::const_iterator it = _edges.begin(); it != _edges.end(); ++it)
		    (*it)->markServiceIndexUpdateNeeded ();
	    }

		void Path::setAllDays( bool value )
		{
			_allDays = value;
		}



		bool Path::getAllDays() const
		{
			return _allDays;
		}

		bool cmpService::operator ()(const Service *s1, const Service *s2) const
		{
			return (s1->getDepartureSchedule () < s2->getDepartureSchedule ())
				|| (s1->getDepartureSchedule () == s2->getDepartureSchedule ()
				&& s1 < s2)						
				;
		}
	}
}
