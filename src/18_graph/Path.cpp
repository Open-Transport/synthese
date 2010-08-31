
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
#include "Log.h"

#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/linearref/LengthIndexedLine.h>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace util;

	namespace graph
	{
		Path::Path():
			RuleUser(),
			_pathGroup(NULL),
			_pathClass(NULL)
		{
		}
		    

		Path::~Path ()
		{
		}



		const RuleUser* Path::_getParentRuleUser() const
		{
			return _pathGroup;
		}


// 		const Service* Path::getService(
// 			int serviceIndex
// 		) const {
// 			ServiceSet::const_iterator it(_services.begin ());
// 			advance (it, serviceIndex);
// 			return (*it);
// 		}




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



		void Path::removeService (Service* service)
		{
			_services.erase(service);

			markScheduleIndexesUpdateNeeded ();
		}



		const Edge* Path::getEdge (int index) const
		{
			return _edges[index];
		}



		void Path::addEdge(
			Edge& edge
		){
			// Empty path : just put the edge in the vector
			if (_edges.empty())
			{
				_edges.push_back(&edge);
				return;
			}

			// Non empty path : determinate the good position of the edge
			Edges::iterator insertionPosition;
			for(insertionPosition = _edges.begin();
				insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() < edge.getRankInPath();
				++insertionPosition) ;

			// If an edge with the same rank exists, then throw an exception
			if(insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() == edge.getRankInPath())
			{
				// If the edge is the same, do nothing
				if((*insertionPosition) == &edge) return;

				throw Exception(
					"An edge with the rank "+ lexical_cast<string>(edge.getRankInPath()) + " already exists in the path " + lexical_cast<string>(getKey())
				);
			}

			// Next arrival and next in path of the previous
			if(insertionPosition != _edges.begin())
			{
				Edge* previousEdge(*(insertionPosition - 1));
				Edge* nextArrival(previousEdge->getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(previousEdge->getFollowingConnectionArrival());

				// Next arrival of previous edges
				if(edge.isArrivalAllowed())
				{
					for(Edges::iterator it(insertionPosition-1); 
						(*it)->getFollowingArrivalForFineSteppingOnly() == nextArrival;
						--it
					){
						(*it)->setFollowingArrivalForFineSteppingOnly(&edge);
						if (it == _edges.begin()) break;
					}

					if(edge.isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition-1); 
							(*it)->getFollowingConnectionArrival() == nextConnectingArrival;
							--it
						){
							(*it)->setFollowingConnectionArrival(&edge);
							if (it == _edges.begin()) break;
						}
					}
				}

				edge.setFollowingArrivalForFineSteppingOnly(nextArrival);
				edge.setFollowingConnectionArrival(nextConnectingArrival);
			}
			else
			{
				// First edge : next arrival is the old first one or its next arrival
				Edge* firstEdge(*insertionPosition);
				if(firstEdge->isArrivalAllowed())
				{
					edge.setFollowingArrivalForFineSteppingOnly(firstEdge);
				}
				else
				{
					edge.setFollowingArrivalForFineSteppingOnly(firstEdge->getFollowingArrivalForFineSteppingOnly());
				}
				if(firstEdge->isArrivalAllowed() && firstEdge->isConnectingEdge())
				{
					edge.setFollowingConnectionArrival(firstEdge);
				}
				else
				{
					edge.setFollowingConnectionArrival(firstEdge->getFollowingConnectionArrival());
				}
			}
		
			// Previous departure
			if(insertionPosition != _edges.end())
			{
				Edge* nextEdge(*insertionPosition);
				Edge* previousDeparture(nextEdge->getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(nextEdge->getPreviousConnectionDeparture());

				// Previous departure of next edges
				if(edge.isDepartureAllowed())
				{
					for(Edges::iterator it(insertionPosition); 
						it != _edges.end() && (*it)->getPreviousDepartureForFineSteppingOnly() == previousDeparture;
						++it
					){
						(*it)->setPreviousDepartureForFineSteppingOnly(&edge);
					}

					if(edge.isConnectingEdge())
					{
						for(Edges::iterator it(insertionPosition); 
							it != _edges.end() && (*it)->getPreviousConnectionDeparture() == previousConnectingDeparture;
							++it
						){
							(*it)->setPreviousConnectionDeparture(&edge);
						}
					}
				}

				edge.setPreviousDepartureForFineSteppingOnly(previousDeparture);
				edge.setPreviousConnectionDeparture(previousConnectingDeparture);
			}
			else
			{
				// Last edge : previous departure is the old last one or its previous departure
				Edge* lastEdge(*(insertionPosition - 1));
				if(lastEdge->isDepartureAllowed())
				{
					edge.setPreviousDepartureForFineSteppingOnly(lastEdge);
				}
				else
				{
					edge.setPreviousDepartureForFineSteppingOnly(lastEdge->getPreviousDepartureForFineSteppingOnly());
				}
				if(lastEdge->isDepartureAllowed() && lastEdge->isConnectingEdge())
				{
					edge.setPreviousConnectionDeparture(lastEdge);
				}
				else
				{
					edge.setPreviousConnectionDeparture(lastEdge->getPreviousConnectionDeparture());
				}
			}

			// Insertion of the new edges
			_edges.insert(insertionPosition, &edge);
		}



		Edge* Path::getLastEdge() const
		{
			if(_edges.empty()) return NULL;
			return *(_edges.end() - 1);
		}



	    void 
	    Path::markScheduleIndexesUpdateNeeded()
	    {
		for (Edges::const_iterator it = _edges.begin(); it != _edges.end(); ++it)
		    (*it)->markServiceIndexUpdateNeeded (false);
	    }



		void Path::merge(Path& other )
		{
			if(	other._pathGroup != _pathGroup ||
				other._edges.empty() ||
				_edges.empty() ||
				other.getEdge(0)->getFromVertex() != getLastEdge()->getFromVertex()
			){
				throw util::Exception("The two roads cannot be merged");
			}

			Edge* lastEdge(getLastEdge());
			Vertex* vertex(const_cast<Vertex*>(lastEdge->getFromVertex()));
			double metricOffset(lastEdge->getMetricOffset());
			int rankInPath(lastEdge->getRankInPath());

			_edges.pop_back();
			vertex->removeArrivalEdge(lastEdge);
			other._edges[0]->setPreviousConnectionDeparture(lastEdge->getPreviousConnectionDeparture());
			other._edges[0]->setPreviousDepartureForFineSteppingOnly(lastEdge->getPreviousDepartureForFineSteppingOnly());

			BOOST_FOREACH(Edge* edge, _edges)
			{
				if(edge->getFollowingConnectionArrival() == lastEdge)
				{
					edge->setFollowingConnectionArrival(other._edges[0]);
				}
				if(edge->getFollowingArrivalForFineSteppingOnly() == lastEdge)
				{
					edge->setFollowingArrivalForFineSteppingOnly(other._edges[0]);
				}
			}

			BOOST_FOREACH(Edge* edge, other._edges)
			{
				Vertex* vertex(const_cast<Vertex*>(edge->getFromVertex()));
				if(edge->isArrivalAllowed())
				{
					vertex->removeArrivalEdge(edge);
				}
				if(edge->isDepartureAllowed())
				{
					vertex->removeDepartureEdge(edge);
				}
				edge->setRankInPath(edge->getRankInPath()+rankInPath);
				edge->setMetricOffset(edge->getMetricOffset()+metricOffset);
				_edges.push_back(edge);
				edge->setParentPath(this);
				if(edge->isArrivalAllowed())
				{
					vertex->addArrivalEdge(edge);
				}
				if(edge->isDepartureAllowed())
				{
					vertex->addDepartureEdge(edge);
				}
			}

			other._edges.clear();

			_pathGroup->removePath(&other);
		}



		double Path::length(
		) const {
			return getLastEdge()->getMetricOffset();
		}



		Edge* Path::getEdgeAtOffset(
			double offset
		) const {
			Edges::const_iterator edgeIt;
			Edge* previous;
			if(offset == 0)
				return _edges.front();
			if(offset == _edges.back()->getMetricOffset())
				return _edges.back();
			for(edgeIt = _edges.begin();edgeIt != _edges.end();++edgeIt) {
				if((*edgeIt)->getMetricOffset() > offset)
					break;
				previous = *edgeIt;
			}
			//assert(edgeIt!=_edges.end());
			return previous;
		}



		bool Path::isRoad() const
		{
			return false;
		}



		bool Path::sameContent( const Path& other, bool considerVertices, bool considerDepartureArrival) const
		{
			if(_edges.size() != other._edges.size())
			{
				return false;
			}

			Edges::const_iterator otherEdgeIt(other._edges.begin());
			BOOST_FOREACH(const Edge* edge, _edges)
			{
				const Edge& otherEdge(**otherEdgeIt);

				if(	(considerVertices && edge->getFromVertex() != otherEdge.getFromVertex()) ||
					edge->getFromVertex()->getHub() != otherEdge.getFromVertex()->getHub() ||
					(considerDepartureArrival && edge->isArrival() != otherEdge.isArrival()) ||
					(considerDepartureArrival && edge->isDeparture() != otherEdge.isDeparture())
				){
					return false;
				}

				++otherEdgeIt;
			}

			return true;
		}



		bool Path::includes( const Path& other, bool considerVertices ) const
		{
			Edges::const_iterator edgeIt(_edges.begin());
			BOOST_FOREACH(const Edge* edge, other._edges)
			{
				for(;
					edgeIt != _edges.end() &&
					(
						(considerVertices && edge->getFromVertex() != (*edgeIt)->getFromVertex()) ||
						edge->getFromVertex()->getHub() != (*edgeIt)->getFromVertex()->getHub() ||
						edge->isArrival() && !(*edgeIt)->isArrival() ||
						edge->isDeparture() && !(*edgeIt)->isDeparture()
					);
					++edgeIt) ;
				if(edgeIt == _edges.end())
				{
					return false;
				}
			}
			return true;
		}



		void Path::removeEdge( Edge& edge )
		{
			// Empty path : just clear the path
			if (_edges.size() == 1)
			{
				_edges.clear();
				return;
			}

			// Non empty path : determinate the position of the edge
			Edges::iterator removalPosition;
			for(removalPosition = _edges.begin();
				removalPosition != _edges.end() && *removalPosition != &edge;
				++removalPosition) ;

			// If edge was not found, nothing to do
			if(removalPosition == _edges.end())
			{
				return;
			}

			// Next arrival and next in path of the previous
			if(removalPosition != _edges.begin() && edge.isArrivalAllowed())
			{
				Edge* nextArrival(edge.getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(edge.getFollowingConnectionArrival());

				for(Edges::iterator it(removalPosition-1); 
					(*it)->getFollowingArrivalForFineSteppingOnly() == &edge;
					--it
				){
					(*it)->setFollowingArrivalForFineSteppingOnly(nextArrival);
					if (it == _edges.begin()) break;
				}

				if(edge.isConnectingEdge())
				{
					for(Edges::iterator it(removalPosition-1); 
						(*it)->getFollowingConnectionArrival() == &edge;
						--it
					){
						(*it)->setFollowingConnectionArrival(nextConnectingArrival);
						if (it == _edges.begin()) break;
					}
				}
			}
		
			// Previous departure
			if(removalPosition != _edges.end() && edge.isDepartureAllowed())
			{
				Edge* previousDeparture(edge.getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(edge.getPreviousConnectionDeparture());

				for(Edges::iterator it(removalPosition + 1); 
					it != _edges.end() && (*it)->getPreviousDepartureForFineSteppingOnly() == &edge;
					++it
				){
					(*it)->setPreviousDepartureForFineSteppingOnly(previousDeparture);
				}

				if(edge.isConnectingEdge())
				{
					for(Edges::iterator it(removalPosition + 1); 
						it != _edges.end() && (*it)->getPreviousConnectionDeparture() == &edge;
						++it
					){
						(*it)->setPreviousConnectionDeparture(previousConnectingDeparture);
					}
				}
			}

			// Insertion of the new edges
			_edges.erase(removalPosition);
		}



		bool cmpService::operator ()(const Service *s1, const Service *s2) const
		{
			return (s1->getDepartureSchedule (false,0) < s2->getDepartureSchedule (false,0))
				|| (s1->getDepartureSchedule (false,0) == s2->getDepartureSchedule (false,0)
				&& s1 < s2)						
			;
		}


		shared_ptr<LineString> Path::getGeometry(
			std::size_t fromEdgeIndex,
			boost::optional<std::size_t> toEdgeIndex
		) const {

			// Geos factories
			const GeometryFactory *gf = GeometryFactory::getDefaultInstance();
			geos::geom::CoordinateSequence *coords(gf->getCoordinateSequenceFactory()->create(0,2));

			// Auto get end edge index
			if(!toEdgeIndex)
			{
				*toEdgeIndex = _edges.size () - 1;
			}

			// Gets the coordinates of each edge
			for(size_t i=fromEdgeIndex; i<=*toEdgeIndex; ++i)
			{
				const CoordinateSequence& geometry(*_edges[i]->getGeometry()->getCoordinatesRO());

				for(size_t j(0); j<geometry.getSize(); ++j)
				{
					coords->add(geometry.getAt(j), false);
				}
			}

			//coords* owned by the shared pointer
			return shared_ptr<LineString>(gf->createLineString(coords));
		}



		double Path::distance(
			const Point& pt
		) const {
			if(pt.isEmpty())
			{
				return std::numeric_limits<double>::infinity();
			}
			return getGeometry()->distance(&pt);
		}



		void Path::validateGeometry() const
		{
			if(!_edges.size()) return;
			double graphlength = _edges.back()->getMetricOffset() - _edges.front()->getMetricOffset();
			shared_ptr<LineString> geometry(getGeometry());
			double geomLength = geometry->getLength();
			if(std::abs(graphlength-geomLength)>0.1)
			{
				std::stringstream ss;
				ss.precision(10);
				ss << "discrepancy found in geometries for path " << getKey() << std::endl;

				for(size_t i(0); i<geometry->getCoordinatesRO()->getSize(); ++i)
				{
					ss << "("<<geometry->getCoordinateN(i).x<< "," << geometry->getCoordinateN(i).y <<")";
				}
				ss<<std::endl<< geometry->toText()<<std::endl;
				util::Log::GetInstance().warn(ss.str());
			}
		}
	}
}
