
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
#include "PathGroup.h"
#include "Edge.h"
#include "Vertex.h"
#include "Service.h"
#include "Exception.h"
#include "Conversion.h"
#include "Hub.h"

#include <assert.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace geometry;
	using namespace time;

	namespace graph
	{

		Path::Path()
			: RuleUser()
			, Calendar()
			, _allDays(false)
			, _pathGroup(NULL)
		{
		}
		    

		Path::~Path ()
		{
		}



		const ServiceSet& Path::getServices(
		) const {
			return _services;
		}



		const RuleUser* Path::_getParentRuleUser() const
		{
			return _pathGroup;
		}


		const Service* Path::getService(
			int serviceIndex
		) const {
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
				throw Exception(
					"Service number " + Conversion::ToString (service->getServiceNumber ())
					+ " is already defined in path " + Conversion::ToString (getKey())
				);
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




		bool Path::isActive(const Date& date) const
		{
			return _allDays || Calendar::isActive(date);
		}



		const Edge* Path::getEdge (int index) const
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



		void Path::addEdge(
			Edge* edge,
			bool autoShift
		){
			// Empty path : just put the edge in the vector

			if (_edges.empty())
			{
				_edges.push_back(edge);
				return;
			}

			// Non empty path : determinate the good position of the edge
			Edges::iterator insertionPosition;
			for(insertionPosition = _edges.begin();
				insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() < edge->getRankInPath();
				++insertionPosition);

			// If an edge with the same rank exists, then throw an exception of shift all ranks after it (depends on the parameter)
			if(insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() == edge->getRankInPath())
			{
				// If the edge is the same, do nothing
				if((*insertionPosition) == edge) return;

				if(!autoShift) throw Exception("An edge with the rank "+ lexical_cast<string>(edge->getRankInPath()) + " already exists in the path " + lexical_cast<string>(getKey()));
				for(Edges::iterator it(insertionPosition+1); it != _edges.end(); ++it)
				{
					(*it)->setRankInPath((*it)->getRankInPath() + 1);
				}
			}

			// Next arrival and next in path of the previous
			if(insertionPosition != _edges.begin())
			{
				Edge* previousEdge(*(insertionPosition - 1));
				Edge* nextArrival(previousEdge->getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(previousEdge->getFollowingConnectionArrival());

				// Next arrival of previous edges
				if(edge->isArrivalAllowed())
				{
					for(Edges::iterator it(insertionPosition-1); 
						(*it)->getFollowingArrivalForFineSteppingOnly() == nextArrival;
						--it
					){
						(*it)->setFollowingArrivalForFineSteppingOnly(edge);
						if (it == _edges.begin()) break;
					}

					if(edge->isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition-1); 
							(*it)->getFollowingConnectionArrival() == nextConnectingArrival;
							--it
						){
							(*it)->setFollowingConnectionArrival(edge);
							if (it == _edges.begin()) break;
						}
					}
				}

				edge->setFollowingArrivalForFineSteppingOnly(nextArrival);
				edge->setFollowingConnectionArrival(nextConnectingArrival);
			}
			else
			{
				// First edge : next arrival is the old first one or its next arrival
				Edge* firstEdge(*insertionPosition);
				if(firstEdge->isArrivalAllowed())
				{
					edge->setFollowingArrivalForFineSteppingOnly(firstEdge);
				}
				else
				{
					edge->setFollowingArrivalForFineSteppingOnly(firstEdge->getFollowingArrivalForFineSteppingOnly());
				}
				if(firstEdge->isArrivalAllowed() && firstEdge->isConnectingEdge())
				{
					edge->setFollowingConnectionArrival(firstEdge);
				}
				else
				{
					edge->setFollowingConnectionArrival(firstEdge->getFollowingConnectionArrival());
				}
			}
		
			// Previous departure
			if(insertionPosition != _edges.end())
			{
				Edge* nextEdge(*insertionPosition);
				Edge* previousDeparture(nextEdge->getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(nextEdge->getPreviousConnectionDeparture());

				// Previous departure of next edges
				if(edge->isDepartureAllowed())
				{
					for(Edges::iterator it(insertionPosition); 
						it != _edges.end() && (*it)->getPreviousDepartureForFineSteppingOnly() == previousDeparture;
						++it
					){
						(*it)->setPreviousDepartureForFineSteppingOnly(edge);
					}

					if(edge->isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition); 
							it != _edges.end() && (*it)->getPreviousConnectionDeparture() == previousConnectingDeparture;
							++it
						){
							(*it)->setPreviousConnectionDeparture(edge);
						}
					}
				}

				edge->setPreviousDepartureForFineSteppingOnly(previousDeparture);
				edge->setPreviousConnectionDeparture(previousConnectingDeparture);
			}
			else
			{
				// Last edge : previous departure is the old last one or its previous departure
				Edge* lastEdge(*(insertionPosition - 1));
				if(lastEdge->isDepartureAllowed())
				{
					edge->setPreviousDepartureForFineSteppingOnly(lastEdge);
				}
				else
				{
					edge->setPreviousDepartureForFineSteppingOnly(lastEdge->getPreviousDepartureForFineSteppingOnly());
				}
				if(lastEdge->isDepartureAllowed() && lastEdge->isConnectingEdge())
				{
					edge->setPreviousConnectionDeparture(lastEdge);
				}
				else
				{
					edge->setPreviousConnectionDeparture(lastEdge->getPreviousConnectionDeparture());
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
