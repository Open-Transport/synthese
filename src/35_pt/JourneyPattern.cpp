
/** JourneyPattern class implementation.
	@file JourneyPattern.cpp

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

#include "JourneyPattern.hpp"

#include "CommercialLine.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "JourneyPatternCopy.hpp"
#include "LineArea.hpp"
#include "Log.h"
#include "Registry.h"
#include "RollingStock.hpp"
#include "TransportNetwork.h"
#include "Service.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace calendar;
	using namespace vehicle;

	namespace util
	{
		template<> const std::string Registry<pt::JourneyPattern>::KEY("JourneyPattern");
	}

	namespace pt
	{
		JourneyPattern::JourneyPattern(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id),
			Path(),
			Named(name),
			Calendar(),
			_directionObj(NULL),
			_isWalkingLine (false),
			_wayBack(false),
			_main(false),
			_plannedLength(0)
		{}



		JourneyPattern::~JourneyPattern ()
		{
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
				delete *it;
		}



		const std::string&
		JourneyPattern::getDirection () const
		{
			return _direction;
		}



		void
		JourneyPattern::setDirection (const std::string& direction)
		{
			_direction = direction;
		}



		const std::string&
		JourneyPattern::getTimetableName () const
		{
			return _timetableName;
		}



		void
		JourneyPattern::setTimetableName (const std::string& timetableName)
		{
			_timetableName = timetableName;
		}



		RollingStock*	JourneyPattern::getRollingStock () const
		{
			return static_cast<RollingStock*>(_pathClass);
		}



		TransportNetwork*   JourneyPattern::getNetwork () const
        {
            return static_cast<TransportNetwork*>(_pathNetwork);
        }



		void JourneyPattern::setRollingStock(RollingStock* rollingStock)
		{
			_pathClass = static_cast<PathClass*>(rollingStock);
		}



		void JourneyPattern::setNetwork(TransportNetwork* transportNetwork)
        {
             _pathNetwork = static_cast<PathClass*>(transportNetwork);
        }



		void JourneyPattern::setWalkingLine (bool isWalkingLine)
		{
			_isWalkingLine = isWalkingLine;
		}



		bool JourneyPattern::getWalkingLine () const
		{
			return _isWalkingLine;
		}



		const StopPoint* JourneyPattern::getOrigin() const
		{
			if (getEdges().empty())
				return NULL;
			return static_cast<const StopPoint*>((*getAllEdges().begin())->getFromVertex());
		}


		const StopPoint* JourneyPattern::getDestination() const
		{
			if (getEdges().empty())
				return NULL;
			return static_cast<const StopPoint*>((*getAllEdges().rbegin())->getFromVertex());
		}



		void JourneyPattern::setCommercialLine(CommercialLine* commercialLine )
		{
			_pathGroup = commercialLine;
		}



		CommercialLine* JourneyPattern::getCommercialLine() const
		{
			return static_cast<CommercialLine*>(_pathGroup);
		}



		bool JourneyPattern::isPedestrianMode() const
		{
			return getWalkingLine();
		}



		int JourneyPattern::addSubLine( JourneyPatternCopy* line )
		{
			SubLines::iterator it(_subLines.insert(_subLines.end(), line));
			return (it - _subLines.begin());
		}



		void JourneyPattern::addService(
			Service& service,
			bool ensureLineTheory
		){
			// Check if the path is consistent
			if(	!_edges.size() ||
				_edges.size() != (*_edges.rbegin())->getRankInPath() + 1
			){
				Log::GetInstance().warn("Service "+ lexical_cast<string>(service.getKey()) +" is not added to the path "+ lexical_cast<string>(getKey()) +" due to inconsistent edges.");
				return;
			}

			/// Test of the respect of the line theory
			/// If OK call the normal Path service insertion
			if (!ensureLineTheory || respectsLineTheory(false, service))
			{
				Path::addService(service, ensureLineTheory);
				return;
			}

			/// If not OK test of the respect of the line theory on each subline and add to it
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
			{
				if ((*it)->addServiceIfCompatible(service))
				{
					return;
				}
			}

			// If no subline can handle the service, create one for it
			JourneyPatternCopy* subline(new JourneyPatternCopy(*this));
			bool isok(subline->addServiceIfCompatible(service));

			assert(isok);
		}



		bool JourneyPattern::respectsLineTheory(
			bool RTData,
			const Service& service
		) const {
			ServiceSet::const_iterator last_it;
			ServiceSet::const_iterator it;
			for(it = _services.begin();
				it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(RTData, 0) < service.getDepartureEndScheduleToIndex(RTData, 0);
				last_it = it++);

			// Same departure time is forbidden
			if (it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(RTData, 0) == service.getDepartureEndScheduleToIndex(RTData, 0))
				return false;

			// Check of the next service if existing
			if (it != _services.end() && !(*it)->respectsLineTheoryWith(RTData, service))
				return false;

			// Check of the previous service if existing
			if (it != _services.begin() && !(*last_it)->respectsLineTheoryWith(RTData, service))
				return false;

			return true;
		}

		const JourneyPattern::SubLines JourneyPattern::getSubLines() const
		{
			return _subLines;
		}



		bool JourneyPattern::operator==(const std::vector<StopPoint*>& stops) const
		{
			if(getEdges().size() != stops.size()) return false;

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				if(static_cast<const LineStop*>(edge)->getFromVertex() != stops[rank]) return false;
				++rank;
			}

			return true;
		}



		bool JourneyPattern::operator==(
			const StopsWithDepartureArrivalAuthorization& stops
		) const	{
			if(getEdges().size() != stops.size())
			{
				return false;
			}

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				const StopsWithDepartureArrivalAuthorization::value_type& stop(stops[rank]);
				if( stop._stop.find(static_cast<StopPoint*>(edge->getFromVertex())) == stop._stop.end() ||
					(((rank > 0 && rank+1 < stops.size() && (edge->isDeparture() != stop._departure)) || edge->isArrival() != stop._arrival)) ||
					((stop._withTimes && dynamic_cast<const DesignatedLinePhysicalStop*>(edge) &&
					  *stop._withTimes != static_cast<const DesignatedLinePhysicalStop*>(edge)->getScheduleInput())) ||
					(stop._metricOffset && stop._metricOffset != edge->getMetricOffset())
				){
					return false;
				}
				++rank;
			}

			return true;
		}



		bool JourneyPattern::operator==( const JourneyPattern& other ) const
		{
			if(getEdges().size() != other.getEdges().size())
			{
				return false;
			}

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				const Edge& otherEdge(*other.getEdge(rank));
				if( edge->getFromVertex() != otherEdge.getFromVertex() ||
					(rank > 0 && edge->isDeparture() != otherEdge.isDeparture()) ||
					(rank+1 < getEdges().size() && edge->isArrival() != otherEdge.isArrival()) ||
					(dynamic_cast<const DesignatedLinePhysicalStop*>(edge) && static_cast<const DesignatedLinePhysicalStop*>(&otherEdge)->getScheduleInput() != static_cast<const DesignatedLinePhysicalStop*>(edge)->getScheduleInput()) ||
					otherEdge.getMetricOffset() != edge->getMetricOffset()
				){
					return false;
				}
				++rank;
			}

			return true;
		}



		const LineStop* JourneyPattern::getLineStop(
			std::size_t rank,
			bool ignoreUnscheduledStops
		) const	{
			if(ignoreUnscheduledStops)
			{
				size_t edgeRank(0);
				BOOST_FOREACH(const Edge* edge, _edges)
				{
					if(	edge->getRankInPath() > 0 &&
						(	!dynamic_cast<const DesignatedLinePhysicalStop*>(edge) ||
							static_cast<const DesignatedLinePhysicalStop*>(edge)->getScheduleInput()
					)	){
						++edgeRank;
					}
					if(rank == edgeRank)
					{
						return static_cast<const LineStop*>(edge);
					}
				}
			}
			return static_cast<const LineStop*>(getEdge(rank));
		}



		bool JourneyPattern::isActive( const boost::gregorian::date& date ) const
		{
			return Calendar::isActive(date);
		}



		std::size_t JourneyPattern::getScheduledStopsNumber() const
		{
			size_t result(0);
			BOOST_FOREACH(const Edge* edge, _edges)
			{
				if(	static_cast<const LineStop&>(*edge).getScheduleInput()
				){
					++result;
				}
			}
			return result;
		}



		std::string JourneyPattern::getRuleUserName() const
		{
			return "Parcours " + getName();
		}



		bool JourneyPattern::callsAtCity( const City& city ) const
		{
			BOOST_FOREACH(Edge* edge, getAllEdges())
			{
				if(	static_cast<StopPoint*>(edge->getFromVertex())->getConnectionPlace()->getCity() == &city
				){
					return true;
				}
			}
			return false;
		}



		bool JourneyPattern::compareStopAreas( const StopsWithDepartureArrivalAuthorization& stops ) const
		{
			if(getEdges().size() != stops.size())
			{
				return false;
			}

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				const StopsWithDepartureArrivalAuthorization::value_type& stop(stops[rank]);

				// Check if a stop can be recognized
				bool ok(false);
				BOOST_FOREACH(const StopWithDepartureArrivalAuthorization::StopsSet::value_type& oStop, stop._stop)
				{
					if(oStop->getHub() == edge->getFromVertex()->getHub())
					{
						ok = true;
						break;
					}
				}
				if(!ok)
				{
					return false;
				}

				if( (rank > 0 && rank+1 < stops.size() && (edge->isDeparture() != stop._departure || edge->isArrival() != stop._arrival)) ||
					((stop._withTimes && dynamic_cast<const DesignatedLinePhysicalStop*>(edge) &&
					  *stop._withTimes != static_cast<const DesignatedLinePhysicalStop*>(edge)->getScheduleInput())) ||
					(stop._metricOffset && stop._metricOffset != edge->getMetricOffset())
				){
					return false;
				}
				++rank;
			}

			return true;
		}



		JourneyPattern::StopWithDepartureArrivalAuthorization::StopWithDepartureArrivalAuthorization(
			const std::set<StopPoint*>& stop,
			boost::optional<MetricOffset> metricOffset /*= boost::optional<MetricOffset>()*/,
			bool departure /*= true*/,
			bool arrival /*= true */,
			boost::optional<bool> withTimes,
			boost::shared_ptr<geos::geom::LineString> geometry
		):	_metricOffset(metricOffset),
			_stop(stop),
			_departure(departure),
			_arrival(arrival),
			_withTimes(withTimes),
			_geometry(geometry)
		{}
}	}
