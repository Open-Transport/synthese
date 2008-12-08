
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

#include "Service.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "CommercialLine.h"
#include "SubLine.h"

namespace synthese
{
	namespace util
	{
		template<> const std::string Registry<env::Line>::KEY("Line");
	}

	namespace env
	{
		Line::Line(
			util::RegistryKeyType id
			, std::string name
			, const Axis* axis
		)	: util::Registrable(id)
			, Path ()
			, _name (name)
			, _axis (axis)
			, _rollingStock (NULL)
			, _isWalkingLine (false)
			, _useInDepartureBoards (true)
			, _useInTimetables (true)
			, _useInRoutePlanning (true)
			, _commercialLine(NULL)
			, _wayBack(boost::logic::indeterminate)
		{	}



		Line::~Line ()
		{
			for (SubLines::const_iterator it(_subLines.begin()); it != _subLines.end(); ++it)
				delete *it;
		}


		const std::string& Line::getName() const
		{
			return _name;
		}


		void Line::setName (const std::string& name)
		{
			_name = name;
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





		const Axis* 
		Line::getAxis () const
		{
			return _axis;
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



		const TransportNetwork* 
		Line::getNetwork () const
		{
			return _commercialLine->getNetwork();
		}



		const RollingStock*
		Line::getRollingStock () const
		{
			return _rollingStock;
		}



		void 
		Line::setRollingStock (const RollingStock* rollingStock)
		{
			_rollingStock = rollingStock;
		}



		void
		Line::setWalkingLine (bool isWalkingLine)
		{
			_isWalkingLine = isWalkingLine;
		}



		bool 
		Line::getWalkingLine () const
		{
			return _isWalkingLine;
		}


		bool 
		Line::isRoad () const
		{
			return false;
		}



		bool 
		Line::isLine () const
		{
			return true;
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

		void Line::setAxis( const Axis* axis )
		{
			_axis = axis;
		}

		void Line::setCommercialLine( const CommercialLine* commercialLine )
		{
			_commercialLine = commercialLine;
		}

		const CommercialLine* Line::getCommercialLine() const
		{
			return _commercialLine;
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

		void Line::addService( Service* service )
		{
			/// Test of the respect of the line theory
			/// If OK call the normal Path service insertion
			if (respectsLineTheory(*service))
			{
				Path::addService(service);
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



		bool Line::respectsLineTheory( const Service& service ) const
		{
			ServiceSet::const_iterator last_it;
			ServiceSet::const_iterator it;
			for(it = _services.begin();
				it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(0) < service.getDepartureEndScheduleToIndex(0);
				last_it = it++);

			// Same departure time is forbidden
			if (it != _services.end() && (*it)->getDepartureBeginScheduleToIndex(0) == service.getDepartureEndScheduleToIndex(0))
				return false;

			// Control of the next service if exists
			if (it != _services.end() && !(*it)->respectsLineTheoryWith(service))
				return false;

			// Control of the previous service if exists
			if (it != _services.begin() && !(*last_it)->respectsLineTheoryWith(service))
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
	}
}
