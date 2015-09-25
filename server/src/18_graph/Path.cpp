
/** Path class implementation.
	@file Path.cpp

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

#include "Path.h"
#include "PathGroup.h"
#include "Edge.h"
#include "Vertex.h"
#include "Service.h"
#include "Exception.h"
#include "Hub.h"
#include "Log.h"
#include "DBModule.h"
#include "CoordinatesSystem.hpp"

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
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace db;

	namespace graph
	{
		Path::Path():
			RuleUser(),
			_pathGroup(NULL),
			_pathClass(NULL),
			_pathNetwork(NULL),
			sharedServicesMutex(new synthese::util::shared_recursive_mutex)
		{}



		Path::~Path ()
		{}



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



		//////////////////////////////////////////////////////////////////////////
		/// Insertion of a service in the path.
		/// @pre the service is not already present in the path (check with ::contains)
		void Path::addService(
			Service& service,
			bool ensureLineTheory
		){
			boost::unique_lock<shared_recursive_mutex> lock(*sharedServicesMutex);

			// Try to insert the service in an existing service collection
			boost::shared_ptr<ChronologicalServicesCollection> insertionCollection;
			if(ensureLineTheory)
			{
				BOOST_FOREACH(ServiceCollections::value_type collection, _serviceCollections)
				{
					if(collection->isCompatible(service))
					{
						insertionCollection = collection;
						break;
					}
				}
			}
			else
			{
				if(!_serviceCollections.empty())
				{
					insertionCollection = *_serviceCollections.begin();
				}
			}

			// If no service collection can host the service, creation of a new collection
			if(!insertionCollection)
			{
				insertionCollection.reset(new ChronologicalServicesCollection);
				_serviceCollections.push_back(insertionCollection);
			}

			insertionCollection->getServices().insert(&service);

			// Reset schedules index
			markScheduleIndexesUpdateNeeded(*insertionCollection, false);
		}



		bool Path::contains( const Service& service ) const
		{
			boost::unique_lock<shared_recursive_mutex> lock(*sharedServicesMutex);
			BOOST_FOREACH(ServiceCollections::value_type collection, _serviceCollections)
			{
				if(collection->getServices().find(const_cast<Service*>(&service)) != collection->getServices().end())
				{
					return true;
				}
			}
			return false;
		}



		void Path::removeService(Service& service)
		{
			boost::unique_lock<shared_recursive_mutex> lock(*sharedServicesMutex);
			for(ServiceCollections::iterator itCollection(_serviceCollections.begin()); itCollection != _serviceCollections.end(); ++itCollection)
			{
				ChronologicalServicesCollection& collection(**itCollection);
				ServiceSet::iterator it(
					collection.getServices().find(&service)
				);
				if(it != collection.getServices().end())
				{
					// Reset schedules index
					markScheduleIndexesUpdateNeeded(collection, false);

					// Service removal
					collection.getServices().erase(it);
					if(collection.getServices().empty())
					{
						_serviceCollections.erase(itCollection);
					}
					break;
				}
			}
		}



		const Edge* Path::getEdge(size_t index) const
		{
			BOOST_FOREACH(const Edges::value_type& edge, _edges)
			{
				if(edge->getRankInPath() == index)
				{
					return edge;
				}
				if(edge->getRankInPath() > index)
				{
					break;
				}
			}

			throw Exception("Edge not found");
		}



		void Path::_linkEdge(
			Edge* previousEdge,
			Edge* nextEdge,
			Edge& edge
		){
			if(!previousEdge && !nextEdge)
			{
				return;
			}

			// Next arrival and next in path of the previous
			edge.setPrevious(previousEdge);
			if(previousEdge)
			{
				Edge* nextArrival(previousEdge->getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(previousEdge->getFollowingConnectionArrival());

				// Next arrival of previous edges
				if(edge.isArrivalAllowed())
				{
					for(Edge* it(previousEdge);
						it && it->getFollowingArrivalForFineSteppingOnly() == nextArrival;
						it = it->getPrevious()
					){
						it->setFollowingArrivalForFineSteppingOnly(&edge);
					}

					if(edge.getFromVertex() &&
						edge.getFromVertex()->getHub() &&
						edge.isConnectingEdge())
					{
						for(Edge* it(previousEdge);
							it && it->getFollowingConnectionArrival() == nextConnectingArrival;
							it = it->getPrevious()
						){
							it->setFollowingConnectionArrival(&edge);
						}
					}
				}

				edge.setFollowingArrivalForFineSteppingOnly(nextArrival);
				edge.setFollowingConnectionArrival(nextConnectingArrival);

				previousEdge->setNext(&edge);
			}
			else
			{
				// First edge : next arrival is the old first one or its next arrival
				if(nextEdge->isArrivalAllowed())
				{
					edge.setFollowingArrivalForFineSteppingOnly(nextEdge);
				}
				else
				{
					edge.setFollowingArrivalForFineSteppingOnly(nextEdge->getFollowingArrivalForFineSteppingOnly());
				}
				if(nextEdge->isArrivalAllowed() && nextEdge->getFromVertex() && nextEdge->getFromVertex()->getHub() && nextEdge->isConnectingEdge())
				{
					edge.setFollowingConnectionArrival(nextEdge);
				}
				else
				{
					edge.setFollowingConnectionArrival(nextEdge->getFollowingConnectionArrival());
				}
			}

			// Previous departure
			edge.setNext(nextEdge);
			if(nextEdge)
			{
				Edge* previousDeparture(nextEdge->getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(nextEdge->getPreviousConnectionDeparture());

				// Previous departure of next edges
				if(edge.isDepartureAllowed())
				{
					for(Edge* it(nextEdge);
						it && it->getPreviousDepartureForFineSteppingOnly() == previousDeparture;
						it = it->getNext()
					){
						it->setPreviousDepartureForFineSteppingOnly(&edge);
					}

					if(edge.getFromVertex() &&
						edge.getFromVertex()->getHub() &&
						edge.isConnectingEdge())
					{
						for(Edge* it(nextEdge);
							it && it->getPreviousConnectionDeparture() == previousConnectingDeparture;
							it = it->getNext()
						){
							it->setPreviousConnectionDeparture(&edge);
						}
					}
				}

				edge.setPreviousDepartureForFineSteppingOnly(previousDeparture);
				edge.setPreviousConnectionDeparture(previousConnectingDeparture);

				nextEdge->setPrevious(&edge);
			}
			else
			{
				// Last edge : previous departure is the old last one or its previous departure
				if(previousEdge->isDepartureAllowed())
				{
					edge.setPreviousDepartureForFineSteppingOnly(previousEdge);
				}
				else
				{
					edge.setPreviousDepartureForFineSteppingOnly(previousEdge->getPreviousDepartureForFineSteppingOnly());
				}
				if(previousEdge->isDepartureAllowed() && previousEdge->getFromVertex() && previousEdge->getFromVertex()->getHub() && previousEdge->isConnectingEdge())
				{
					edge.setPreviousConnectionDeparture(previousEdge);
				}
				else
				{
					edge.setPreviousConnectionDeparture(previousEdge->getPreviousConnectionDeparture());
				}
			}
		}



		void Path::addEdge(
			Edge& edge
		){
			// Empty path : just put the edge in the vector
			if (_edges.empty())
			{
				_linkEdge(NULL, NULL, edge);
				_edges.push_back(&edge);
				_rankMap.insert(make_pair(edge.getMetricOffset(), edge.getRankInPath()));
				return;
			}

			// Non empty path : determinate the good position of the edge
			Edges::iterator insertionPosition;
			for(insertionPosition = _edges.begin();
				insertionPosition != _edges.end() && (*insertionPosition)->getRankInPath() <= edge.getRankInPath();
				++insertionPosition) ;

			// Builds the links between edges
			Edge* previousEdge(NULL);
			if(insertionPosition != _edges.begin())
			{
				previousEdge = 
					insertionPosition != _edges.end() ?
					*(insertionPosition - 1) :
					*_edges.rbegin()
				;
			}
			Edge* nextEdge(NULL);
			if(insertionPosition != _edges.end())
			{
				nextEdge = *insertionPosition;
			}
			_linkEdge(previousEdge, nextEdge, edge);

			// Insertion of the new edges
			_edges.insert(insertionPosition, &edge);

			// Update of the rank map
			RankMap::iterator itEdge(_rankMap.find(edge.getMetricOffset()));
			if(itEdge == _rankMap.end())
			{
				_rankMap.insert(make_pair(edge.getMetricOffset(), edge.getRankInPath()));
			}
			else
			{
				if(itEdge->second > edge.getRankInPath())
				{
					itEdge->second = edge.getRankInPath();
				}
			}
		}



		Edge* Path::getLastEdge() const
		{
			if(_edges.empty()) return NULL;
			return *(_edges.end() - 1);
		}



		void Path::markScheduleIndexesUpdateNeeded(
			const ChronologicalServicesCollection& collection,
			bool RTDataOnly
		) const	{
			BOOST_FOREACH(const Edges::value_type& edge, _edges)
			{
				edge->markServiceIndexUpdateNeeded(collection, RTDataOnly);
			}
		}



		double Path::length(
		) const {
			return getLastEdge()->getMetricOffset();
		}



		Edge* Path::getEdgeAtOffset(
			double offset
		) const {
			Edges::const_iterator edgeIt;
			Edge* previous = NULL;
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
						((considerVertices && edge->getFromVertex() != (*edgeIt)->getFromVertex())) ||
						edge->getFromVertex()->getHub() != (*edgeIt)->getFromVertex()->getHub() ||
						(edge->isArrival() && !(*edgeIt)->isArrival()) ||
						(edge->isDeparture() && !(*edgeIt)->isDeparture())
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

			Edge* lastRealEdge(edge.getPrevious());
			Edge* firstRealEdge(edge.getNext());

			// Next arrival pointers
			if(removalPosition != _edges.begin() && edge.isArrivalAllowed() && lastRealEdge)
			{
				Edge* nextArrival(edge.getFollowingArrivalForFineSteppingOnly());
				Edge* nextConnectingArrival(edge.getFollowingConnectionArrival());

				{
					Edge* oldNextArrival(lastRealEdge->getFollowingArrivalForFineSteppingOnly());

					for(Edge* it(lastRealEdge);
						it && it->getFollowingArrivalForFineSteppingOnly() == oldNextArrival;
						it = it->getPrevious()
					){
						it->setFollowingArrivalForFineSteppingOnly(nextArrival);
				}	}

				{
					Edge* oldNextConnectionArrival(lastRealEdge->getFollowingConnectionArrival());

					for(Edge* it(lastRealEdge);
						it && it->getFollowingConnectionArrival() == oldNextConnectionArrival;
						it = it->getPrevious()
					){
						it->setFollowingConnectionArrival(nextConnectingArrival);
				}	}
			}

			// Next in path of the previous
			if(lastRealEdge)
			{
				lastRealEdge->setNext(firstRealEdge);
			}

			// Previous departure pointers
			if(removalPosition+1 != _edges.end() && edge.isDepartureAllowed() && firstRealEdge)
			{
				Edge* previousDeparture(edge.getPreviousDepartureForFineSteppingOnly());
				Edge* previousConnectingDeparture(edge.getPreviousConnectionDeparture());

				{
					Edge* oldPreviousDeparture(firstRealEdge->getPreviousDepartureForFineSteppingOnly());

					for(Edge* it(firstRealEdge);
						it && it->getPreviousDepartureForFineSteppingOnly() == oldPreviousDeparture;
						it = it->getNext()
					){
						it->setPreviousDepartureForFineSteppingOnly(previousDeparture);
				}	}

				{
					Edge* oldPreviousConnectionDeparture(firstRealEdge->getPreviousConnectionDeparture());

					for(Edge* it(firstRealEdge);
						it && it->getPreviousConnectionDeparture() == oldPreviousConnectionDeparture;
						it = it->getNext()
					){
						it->setPreviousConnectionDeparture(previousConnectingDeparture);
				}	}
			}

			// Previous in path of the next
			if(firstRealEdge)
			{
				firstRealEdge->setPrevious(lastRealEdge);
			}

			// Update of the rank map
			RankMap::iterator it(_rankMap.find(edge.getMetricOffset()));
			if(it != _rankMap.end())
			{
				_rankMap.erase(it);
			}

			// Removal of the edge
			_edges.erase(removalPosition);

			// Search of other edge with the same metric offset to record in rank map
			BOOST_FOREACH(Edge* itEdge, _edges)
			{
				if(itEdge->getMetricOffset() == edge.getMetricOffset())
				{
					_rankMap[itEdge->getMetricOffset()] = itEdge->getRankInPath();
					break;
				}
				if(itEdge->getMetricOffset() > edge.getMetricOffset())
				{
					break;
				}
			}
		}



		boost::shared_ptr<LineString> Path::getGeometry(
			std::size_t fromEdgeIndex,
			boost::optional<std::size_t> toEdgeIndex
		) const {

			// Geos factories
			const GeometryFactory& gf(CoordinatesSystem::GetDefaultGeometryFactory());
			geos::geom::CoordinateSequence *coords(gf.getCoordinateSequenceFactory()->create((size_t)0, 2));

			// Handle empty roads
			if(_edges.empty())
			{
				return boost::shared_ptr<LineString>(gf.createLineString());
			}

			// Auto get end edge index
			if(!toEdgeIndex)
			{
				toEdgeIndex = _edges.size () - 1;
			}

			// Gets the coordinates of each edge
			for(size_t i=fromEdgeIndex; i<=*toEdgeIndex; ++i)
			{
				boost::shared_ptr<LineString> geometry(_edges[i]->getRealGeometry());
				if(!geometry.get())
				{
					break;
				}

				for(size_t j(0); j<geometry->getCoordinatesRO()->getSize(); ++j)
				{
					coords->add(geometry->getCoordinateN(j), false);
				}
			}

			//coords* owned by the shared pointer
			return boost::shared_ptr<LineString>(gf.createLineString(coords));
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
			boost::shared_ptr<LineString> geometry(getGeometry());
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



		std::size_t Path::getEdgeRankAtOffset( MetricOffset offset ) const
		{
			RankMap::const_iterator it(_rankMap.find(offset));
			if(it == _rankMap.end())
			{
				throw InvalidOffsetException(offset, *this);
			}
			return it->second;
		}



		const Edge& Path::findEdgeByVertex( const Vertex* vertex ) const
		{
			Vertex::Edges::const_iterator it(vertex->getDepartureEdges().find(this));
			if(it != vertex->getDepartureEdges().end())
			{
				return *it->second;
			}
			Vertex::Edges::const_iterator it2(vertex->getArrivalEdges().find(this));
			if(it2 != vertex->getArrivalEdges().end())
			{
				return *it2->second;
			}
			throw VertexNotFoundException();
		}



		void Path::markAllScheduleIndexesUpdateNeeded( bool RTDataOnly ) const
		{
			BOOST_FOREACH(const ServiceCollections::value_type& itCollection, _serviceCollections)
			{
				markScheduleIndexesUpdateNeeded(*itCollection, RTDataOnly);
			}
		}



		synthese::graph::ServiceSet Path::getAllServices() const
		{
			ServiceSet result;
			BOOST_FOREACH(const ServiceCollections::value_type& itCollection, _serviceCollections)
			{
				result.insert(itCollection->getServices().begin(), itCollection->getServices().end());
			}
			return result;
		}



		const Vertex* Path::getDestination() const
		{
			if(_edges.empty())
			{
				return NULL;
			}
			return (*_edges.rbegin())->getFromVertex();
		}



		const Vertex* Path::getOrigin() const
		{
			if (_edges.empty())
			{
				return NULL;
			}
			return (*_edges.begin())->getFromVertex();
		}



		Path::InvalidOffsetException::InvalidOffsetException(
			MetricOffset offset,
			const Path& path
		):	synthese::Exception("Invalid offset "+ lexical_cast<string>(offset) + " in path "+ lexical_cast<string>(path.getKey()))
		{}
}	}
