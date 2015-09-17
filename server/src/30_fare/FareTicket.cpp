
/** FareTicket class implementation.
	@file FareTicket.cpp

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

#include "FareTicket.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace fare
	{
		FareTicket::FareTicket(const graph::ServicePointer* firstService, const Fare* fare)
			:	_fare(fare),
				_price(0),
				_numberOfConnections(0),
				_firstService(firstService),
				_lastService(firstService),
				_available(true)
		{
			if((fare != NULL) && (fare->getType()))
			{
				_price = fare->getType()->getAccessPrice();
			}
			if(firstService != NULL)
			{
				_startTime = firstService->getDepartureDateTime();
				_endTime = firstService->getArrivalDateTime();
			}
		}



		bool FareTicket::isAvailable(const graph::ServicePointer* newService, const graph::ServicePointer* lastService)
		{
			if((newService == NULL) || (_fare == NULL) || (!_available))
				return false;

			// test continuity
			if(_fare->get<RequiredContinuity>()
				&& lastService != NULL
				&& lastService != getLastService())
			{
				return false;
			}

			// test the number of permitted connections
			if(_fare->get<PermittedConnectionsNumber>() != 0
				&& _fare->get<PermittedConnectionsNumber>() <= getNumberOfConnections())
			{
				return false;
			}

			// test validity period of the ticket
			boost::posix_time::time_duration period = newService->getArrivalDateTime() - getStartTime();
			if(_fare->get<ValidityPeriod>() < period.minutes())
				return false;

			return true;
		}



		void FareTicket::useService(const graph::ServicePointer* service)
		{
			if((service == NULL) || (!_available))
				return;

			_lastService = service;
			_endTime = service->getArrivalDateTime();
			_numberOfConnections++;

			// TODO update _price according to the FareType
		}
	}
}
