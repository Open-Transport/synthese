
/** ServicePointer class implementation.
	@file ServicePointer.cpp

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

#include "15_env/ServicePointer.h"
#include "15_env/Service.h"
#include "15_env/ReservationRule.h"

#include "04_time/module.h"

#include "01_util/Constants.h"

namespace synthese
{
	using namespace time;

	namespace env
	{

		ServicePointer::ServicePointer(DeterminationMethod method, const Edge* edge)
			: _service(NULL)
			, _originDateTime(TIME_UNKNOWN)
			, _serviceIndex(UNKNOWN_VALUE)
			, _determinationMethod(method)
			, _actualTime(TIME_UNKNOWN)
			, _range(0)
			, _edge(edge)
		{

		}

		ServicePointer::ServicePointer()
			: _determinationMethod(NULL_POINTER)
			, _service(NULL)
			, _originDateTime(TIME_UNKNOWN)
			, _serviceIndex(UNKNOWN_VALUE)
			, _actualTime(TIME_UNKNOWN)
			, _range(0)
			, _edge(NULL) 
		{

		}
		void ServicePointer::setActualTime( const time::DateTime& dateTime )
		{
			_actualTime = dateTime;
		}

		void ServicePointer::setService( const Service* service )
		{
			_service = service;
		}

		void ServicePointer::setOriginDateTime( const time::DateTime& dateTime )
		{
			_originDateTime = dateTime;
		}

		void ServicePointer::setServiceIndex( int index )
		{
			_serviceIndex = index;
		}

		bool ServicePointer::isReservationRuleCompliant(const DateTime& computingDateTime) const
		{
			if (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				return true;

			if (_service->getReservationRule()->isCompliant() == true)
				return _service->getReservationRule()->isRunPossible(_originDateTime, computingDateTime, _actualTime);

			return true;
		}

		const Service* ServicePointer::getService() const
		{
			return _service;
		}

		const time::DateTime& ServicePointer::getActualDateTime() const
		{
			return _actualTime;
		}

		const time::DateTime& ServicePointer::getOriginDateTime() const
		{
			return _originDateTime;
		}

		ServicePointer::DeterminationMethod ServicePointer::getMethod() const
		{
			return _determinationMethod;
		}

		const env::Edge* ServicePointer::getEdge() const
		{
			return _edge;
		}

		int ServicePointer::getServiceIndex() const
		{
			return _serviceIndex;
		}

		void ServicePointer::setServiceRange(int duration)
		{
			_range = duration;
		}

		int ServicePointer::getServiceRange() const
		{
			return _range;
		}
	}
}
