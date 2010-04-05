
/** Line class implementation.
	@file Line.cpp

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

#include "Line.h"
#include "Registry.h"
#include "RollingStock.h"
#include "Service.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "CommercialLine.h"
#include "SubLine.h"

#include <boost/foreach.hpp>

namespace synthese
{
	using namespace graph;
	using namespace util;
	using namespace impex;
	using namespace pt;
	
	namespace util
	{
		template<> const std::string Registry<pt::Line>::KEY("Line");
	}

	namespace pt
	{
		Line::Line(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id)
			, Path(),
			Importable(),
			Named(name)
			, _isWalkingLine (false)
			, _useInDepartureBoards (true)
			, _useInTimetables (true)
			, _useInRoutePlanning (true)
			, _wayBack(boost::logic::indeterminate)
		{	}



		Line::~Line ()
		{
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
				delete *it;
		}




		bool 
		Line::getUseInDepartureBoards () const
		{
			return _useInDepartureBoards;
		}



		void 
		Line::setUseInDepartureBoards (bool useInDepartureBoards)
		{
			_useInDepartureBoards = useInDepartureBoards;
		}




		bool 
		Line::getUseInTimetables () const
		{
			return _useInTimetables;
		}



		void 
		Line::setUseInTimetables (bool useInTimetables)
		{
			_useInTimetables = useInTimetables;
		}




		bool 
		Line::getUseInRoutePlanning () const
		{
			return _useInRoutePlanning;
		}




		void 
		Line::setUseInRoutePlanning (bool useInRoutePlanning)
		{
			_useInRoutePlanning = useInRoutePlanning;
		}



		const std::string& 
		Line::getDirection () const
		{
			return _direction;
		}



		void 
		Line::setDirection (const std::string& direction)
		{
			_direction = direction;
		}



		const std::string& 
		Line::getTimetableName () const
		{
			return _timetableName;
		}



		void 
		Line::setTimetableName (const std::string& timetableName)
		{
			_timetableName = timetableName;
		}



		RollingStock*	Line::getRollingStock () const
		{
			return static_cast<RollingStock*>(_pathClass);
		}



		void Line::setRollingStock(RollingStock* rollingStock)
		{
			_pathClass = static_cast<PathClass*>(rollingStock);
		}



		void Line::setWalkingLine (bool isWalkingLine)
		{
			_isWalkingLine = isWalkingLine;
		}



		bool Line::getWalkingLine () const
		{
			return _isWalkingLine;
		}



		const PhysicalStop* Line::getOrigin() const
		{
			if (getEdges().empty())
				return NULL;
		    return static_cast<const PhysicalStop*>((getEdges().at (0))->getFromVertex());
		}


		const PhysicalStop* Line::getDestination() const
		{
			if (getEdges().empty())
				return NULL;
			Edge* edge = getLastEdge();
			return static_cast<const PhysicalStop*>(edge->getFromVertex());
		}



		void Line::setCommercialLine(CommercialLine* commercialLine )
		{
			_pathGroup = commercialLine;
		}

		CommercialLine* Line::getCommercialLine() const
		{
			return static_cast<CommercialLine*>(_pathGroup);
		}

		bool Line::isPedestrianMode() const
		{
			return getWalkingLine();
		}

		int Line::addSubLine( SubLine* line )
		{
			SubLines::iterator it(_subLines.insert(_subLines.end(), line));
			return (it - _subLines.begin());
		}

		void Line::addService(
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
			SubLine* subline(new SubLine(this));
			bool isok(subline->addServiceIfCompatible(service));

			assert(isok);
		}



		bool Line::respectsLineTheory(
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

		const Line::SubLines Line::getSubLines() const
		{
			return _subLines;
		}

		boost::logic::tribool Line::getWayBack() const
		{
			return _wayBack;
		}

		void Line::setWayBack( boost::logic::tribool value )
		{
			_wayBack = value;
		}
		
		
		
		bool Line::operator==(const std::vector<PhysicalStop*> stops) const
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



		const LineStop* Line::getLineStop( std::size_t rank ) const
		{
			return static_cast<const LineStop*>(getEdge(rank));
		}
	}
}
