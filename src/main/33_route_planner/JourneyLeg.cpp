
/** JourneyLeg class implementation.
	@file JourneyLeg.cpp

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

#include "33_route_planner/JourneyLeg.h"

#include "15_env/ContinuousService.h"
#include "15_env/Edge.h"
#include "15_env/Path.h"

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace routeplanner
	{

		JourneyLeg::JourneyLeg (const ServicePointer& servicePointer)
			: _servicePointer(servicePointer)
			, _origin (0)
			, _destination (0)
			, _departureTime ((servicePointer.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL) ? servicePointer.getActualDateTime() : DateTime(TIME_UNKNOWN))
			, _arrivalTime ((servicePointer.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE) ? servicePointer.getActualDateTime() : DateTime(TIME_UNKNOWN))
			, _continuousServiceRange (0)
		{
		}



		JourneyLeg::~JourneyLeg ()
		{
		}



		const ServicePointer& JourneyLeg::getServiceInstance() const
		{
			return _servicePointer;
		}


		const Axis* 
		JourneyLeg::getAxis () const
		{
			return _servicePointer.getService()->getPath ()->getAxis ();
		}



		const synthese::time::DateTime& 
		JourneyLeg::getDepartureTime () const
		{
			return _departureTime;
		}


		void 
		JourneyLeg::setDepartureTime (const synthese::time::DateTime& departureTime)
		{
			_departureTime = departureTime;
		}



		const synthese::time::DateTime& 
		JourneyLeg::getArrivalTime () const
		{
			return _arrivalTime;
		}



		void 
		JourneyLeg::setArrivalTime (const synthese::time::DateTime& arrivalTime)
		{
			_arrivalTime = arrivalTime;
		}



		const synthese::env::Edge* 
		JourneyLeg::getOrigin () const
		{
			return _origin;
		}




		void 
		JourneyLeg::setOrigin (const synthese::env::Edge* origin)
		{
			_origin = origin;
		}





		const synthese::env::Edge* 
		JourneyLeg::getDestination () const
		{
			return _destination;
		}
		    



		void 
		JourneyLeg::setDestination (const synthese::env::Edge* destination)
		{
			_destination = destination;
		}





		int 
		JourneyLeg::getContinuousServiceRange () const
		{
			return _continuousServiceRange;
		}




		void 
		JourneyLeg::setContinuousServiceRange (int continuousServiceRange)
		{
			_continuousServiceRange = continuousServiceRange;
		}





		const SquareDistance& 
		JourneyLeg::getSquareDistance () const
		{
			return _squareDistance;
		}




		synthese::env::SquareDistance& 
		JourneyLeg::getSquareDistance ()
		{
			return _squareDistance;
		}




		void 
		JourneyLeg::setSquareDistance (const SquareDistance& squareDistance)
		{
			_squareDistance.setSquareDistance (squareDistance.getSquareDistance ());
		}




		int 
		JourneyLeg::getDuration () const
		{
			return _arrivalTime - _departureTime;
		}




		int 
		JourneyLeg::getDistance () const
		{
			return _destination->getMetricOffset () -
			_origin->getMetricOffset ();
		}



		const Path* 
		JourneyLeg::getPath () const
		{
			return _origin->getParentPath ();
		}
	}
}
