
/** ServiceUse class implementation.
	@file ServiceUse.cpp

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

#include "ServiceUse.h"

namespace synthese
{
	namespace env
	{


		ServiceUse::ServiceUse( const ServicePointer& servicePointer, const Edge* edge)
			: _servicePointer(servicePointer)
			, _edge(edge)
		{
			
		}

		const env::Edge* ServiceUse::getDepartureEdge() const
		{
			return (_servicePointer.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
				? _servicePointer.getEdge()
				: _edge;
		}

		const env::Edge* ServiceUse::getArrivalEdge() const
		{
			return (_servicePointer.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE)
				? _servicePointer.getEdge()
				: _edge;
		}

		const time::DateTime& ServiceUse::getDepartureDateTime() const
		{
			return  (_servicePointer.getMethod() == ServicePointer::DEPARTURE_TO_ARRIVAL)
				? _servicePointer.getActualDateTime()
				: _actualDateTime;
		}

		const time::DateTime& ServiceUse::getArrivalDateTime() const
		{
			return  (_servicePointer.getMethod() == ServicePointer::ARRIVAL_TO_DEPARTURE)
				? _servicePointer.getActualDateTime()
				: _actualDateTime;
		}

		const env::Edge* ServiceUse::getEdge() const
		{
			return _edge;
		}

		const ServicePointer& ServiceUse::getServicePointer() const
		{
			return _servicePointer;
		}

		const time::DateTime& ServiceUse::getActualDateTime() const
		{
			return _actualDateTime;
		}
	}
}
