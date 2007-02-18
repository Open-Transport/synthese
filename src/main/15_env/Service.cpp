
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

#include "Service.h"

#include "Path.h"

namespace synthese
{
	namespace env
	{
		Service::Service (int serviceNumber,
				Path* path,
				const synthese::time::Schedule& departureSchedule)
			: BikeComplyer (path) 
			, HandicappedComplyer (path) 
			, PedestrianComplyer (path) 
			, _serviceNumber (serviceNumber)
			, _path (path)
			, _departureSchedule (departureSchedule)
		{
		}


		Service::Service()
		: BikeComplyer(NULL)
		, HandicappedComplyer(NULL)
		, PedestrianComplyer(NULL)
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
		Service::getPath () const
		{
			return _path;
		}



		Calendar&
		Service::getCalendar ()
		{
			return _calendar;
		}



		const synthese::time::Schedule&
		Service::getDepartureSchedule () const
		{
			return _departureSchedule;
		}



		void 
		Service::setDepartureSchedule (const synthese::time::Schedule& departureSchedule)
		{
			_departureSchedule = departureSchedule;
		}



		bool 
		Service::isReservationPossible ( const synthese::time::DateTime& departureMoment, 
						const synthese::time::DateTime& calculationMoment ) const
		{
			return false;
		}



		bool 
		Service::isProvided ( const synthese::time::Date& departureDate,
					int jplus ) const
		{
			if ( jplus > 0 )
			{
				synthese::time::Date _originDepartureDate;
				_originDepartureDate = departureDate;
				_originDepartureDate -= jplus;
				return _calendar.isMarked( _originDepartureDate );
			}
			else
				return _calendar.isMarked ( departureDate );
		}

		void Service::setPath( Path* path )
		{
			_path = path;
			BikeComplyer::setParent(path);
			HandicappedComplyer::setParent(path);
			PedestrianComplyer::setParent(path);
		}
	}
}
