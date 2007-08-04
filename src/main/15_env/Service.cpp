
/** service class implementation.
	@file service.cpp

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

#include "15_env/Service.h"
#include "15_env/Path.h"
#include "15_env/Edge.h"

namespace synthese
{
	using namespace time;

	namespace env
	{
		Service::Service (int serviceNumber,
				Path* path
		)
			: Complyer () 
			, _serviceNumber (serviceNumber)
			, _path (path)
		{
			setComplianceParent(path);
		}


		Service::Service()
		: Complyer()
		, _serviceNumber(0)
		, _path(NULL)
		{

		}


		Service::~Service ()
		{
		}



		int
		Service::getServiceNumber () const
		{
			return _serviceNumber;
		}



		void 
		Service::setServiceNumber (int serviceNumber)
		{
			_serviceNumber = serviceNumber;
		}




		Path* 
		Service::getPath ()
		{
			return _path;
		}

		const Path* 
			Service::getPath () const
		{
			return _path;
		}



		bool 
		Service::isReservationPossible ( const synthese::time::DateTime& departureMoment, 
						const synthese::time::DateTime& calculationMoment ) const
		{
			return false;
		}



		void Service::setPath( Path* path )
		{
			_path = path;
			setComplianceParent(path);
		}

/*		const Schedule& Service::getLastArrivalSchedule() const
		{
			return _arrivalSchedule;
		}

		void Service::setArrivalSchedule( const synthese::time::Schedule& schedule )
		{
			_arrivalSchedule = schedule;
		}
*/
		DateTime Service::getOriginDateTime(const Date& departureDate, const Schedule& departureTime) const
		{
			DateTime originDateTime(departureDate);
			originDateTime -= (departureTime.getDaysSinceDeparture() - getDepartureSchedule().getDaysSinceDeparture());
			originDateTime.setHour(getDepartureSchedule().getHour());
			return originDateTime;
		}
	}
}
