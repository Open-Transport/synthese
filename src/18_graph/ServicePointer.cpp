
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

#include "ServicePointer.h"
#include "Service.h"
#include "UseRule.h"

#include "04_time/module.h"

#include "01_util/Constants.h"

namespace synthese
{
	using namespace time;

	namespace graph
	{

		ServicePointer::ServicePointer(
			AccessDirection method,
			UserClassCode userClassCode,
			const Edge* edge
		):	_service(NULL)
			, _originDateTime(TIME_UNKNOWN)
			, _serviceIndex(UNKNOWN_VALUE)
			, _determinationMethod(method)
			, _actualTime(TIME_UNKNOWN)
			, _range(0)
			, _edge(edge),
			_userClass(userClassCode),
			_useRule(UseRule::ACCESS_UNKNOWN)
		{
		}



		const UseRule& ServicePointer::getUseRule(
		) const {
			return _useRule;
		}
		
		

		void ServicePointer::setActualTime( const time::DateTime& dateTime )
		{
			_actualTime = dateTime;
		}

		void ServicePointer::setService( const Service* service )
		{
			_service = service;
			_useRule = service->getUseRule(_userClass);
		}

		void ServicePointer::setOriginDateTime( const time::DateTime& dateTime )
		{
			_originDateTime = dateTime;
		}

		void ServicePointer::setServiceIndex( int index )
		{
			_serviceIndex = index;
		}

		bool ServicePointer::isReservationRuleCompliant(
			const DateTime& computingDateTime
		) const	{
			if (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				return true;

			UseRule::ReservationRuleType reservationRuleType(_useRule.getReservationType());
			if(reservationRuleType != UseRule::RESERVATION_FORBIDDEN)
			{
				return _useRule.isRunPossible(
					_originDateTime,
					(reservationRuleType == UseRule::RESERVATION_MIXED_BY_DEPARTURE_PLACE) ? false : true,
					computingDateTime,
					_actualTime
				);
			}
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

		AccessDirection ServicePointer::getMethod() const
		{
			return _determinationMethod;
		}

		const Edge* ServicePointer::getEdge() const
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
		
		UserClassCode ServicePointer::getUserClass() const
		{
			return _userClass;
		}
	}
}
