
/** JourneyPattern class implementation.
	@file JourneyPattern.cpp

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

#include "JourneyPattern.hpp"
#include "Registry.h"
#include "RollingStock.h"
#include "Service.h"
#include "LineArea.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "StopPoint.hpp"
#include "CommercialLine.h"
#include "JourneyPatternCopy.hpp"

#include <boost/foreach.hpp>

namespace synthese
{
	using namespace graph;
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace calendar;
	
	namespace util
	{
		template<> const std::string Registry<pt::JourneyPattern>::KEY("JourneyPattern");
	}

	namespace pt
	{
		JourneyPattern::JourneyPattern(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id)
			, Path(),
			Importable(),
			Named(name),
			Calendar(),
			_isWalkingLine (false)
			, _useInDepartureBoards (true)
			, _useInTimetables (true)
			, _useInRoutePlanning (true)
			, _wayBack(false)
		{	}



		JourneyPattern::~JourneyPattern ()
		{
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
				delete *it;
		}




		bool 
		JourneyPattern::getUseInDepartureBoards () const
		{
			return _useInDepartureBoards;
		}



		void 
		JourneyPattern::setUseInDepartureBoards (bool useInDepartureBoards)
		{
			_useInDepartureBoards = useInDepartureBoards;
		}




		bool 
		JourneyPattern::getUseInTimetables () const
		{
			return _useInTimetables;
		}



		void 
		JourneyPattern::setUseInTimetables (bool useInTimetables)
		{
			_useInTimetables = useInTimetables;
		}




		bool 
		JourneyPattern::getUseInRoutePlanning () const
		{
			return _useInRoutePlanning;
		}




		void 
		JourneyPattern::setUseInRoutePlanning (bool useInRoutePlanning)
		{
			_useInRoutePlanning = useInRoutePlanning;
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



		void JourneyPattern::setRollingStock(RollingStock* rollingStock)
		{
			_pathClass = static_cast<PathClass*>(rollingStock);
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
			Service* service,
			bool ensureLineTheory
		){
			/// Test of the respect of the line theory
			/// If OK call the normal Path service insertion
			if (!ensureLineTheory || respectsLineTheory(false, *service))
			{
				Path::addService(service, ensureLineTheory);
				return;
			}

			/// If not OK test of the respect of the line theory on each subline and add to it
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
			{
				if ((*it)->addServiceIfCompatible(service))
					return;
			}
		
			// If no subline can handle the service, create one for it
			JourneyPatternCopy* subline(new JourneyPatternCopy(this));
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

			// Control of the next service if exists
			if (it != _services.end() && !(*it)->respectsLineTheoryWith(RTData, service))
				return false;

			// Control of the previous service if exists
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
					(rank > 0 && rank+1 < stops.size() && (edge->isDeparture() != stop._departure || edge->isArrival() != stop._arrival)) ||
					(dynamic_cast<const DesignatedLinePhysicalStop*>(edge) && stop._withTimes != static_cast<const DesignatedLinePhysicalStop*>(edge)->getScheduleInput())
				){
					return false;
				}
				++rank;
			}

			return true;
		}


		const LineStop* JourneyPattern::getLineStop( std::size_t rank ) const
		{
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


		JourneyPattern::StopWithDepartureArrivalAuthorization::StopWithDepartureArrivalAuthorization(
			const std::set<StopPoint*>& stop,
			boost::optional<Edge::MetricOffset> metricOffset /*= boost::optional<MetricOffset>()*/,
			bool departure /*= true*/,
			bool arrival /*= true */,
			bool withTimes
		):	_stop(stop),
			_metricOffset(metricOffset),
			_departure(departure),
			_arrival(arrival),
			_withTimes(withTimes)
		{}
}	}
