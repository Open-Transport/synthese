
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

#include "Service.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "15_env/CommercialLine.h"

namespace synthese
{
	namespace util
	{
		template<> typename Registrable<uid,env::Line>::Registry Registrable<uid,env::Line>::_registry;
	}

	namespace env
	{
		Line::Line (const uid& id,
				const std::string& name, 
				const Axis* axis)
			: synthese::util::Registrable<uid,Line> (id)
			, Path ()
			, _name (name)
			, _axis (axis)
			, _rollingStock (NULL)
			, _isWalkingLine (false)
			, _useInDepartureBoards (true)
			, _useInTimetables (true)
			, _useInRoutePlanning (true)
			, _commercialLine(NULL)
		{

		}

		Line::Line()
		: synthese::util::Registrable<uid,Line>()
		, Path ()
		, _rollingStock (NULL)
		, _isWalkingLine (false)
		, _useInDepartureBoards (true)
		, _useInTimetables (true)
		, _useInRoutePlanning (true)
		, _commercialLine(NULL)
		{

		}




		Line::~Line ()
		{
		}



		const std::string& 
		Line::getName () const
		{
			return _name;
		}



		void 
		Line::setName (const std::string& name)
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




		uid	Line::getId () const
		{
			return synthese::util::Registrable<uid,Line>::getKey();
		}

		const PhysicalStop* Line::getDestination() const
		{
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
	}
}
