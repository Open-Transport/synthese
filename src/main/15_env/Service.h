
/** service class header.
	@file service.h

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

#ifndef SYNTHESE_ENV_SERVICE_H
#define SYNTHESE_ENV_SERVICE_H

#include "BikeComplyer.h"
#include "HandicappedComplyer.h"
#include "PedestrianComplyer.h"

#include <string>

#include "01_util/UId.h"

#include "04_time/Schedule.h"

#include "15_env/Calendar.h"

namespace synthese
{
	namespace time
	{
		class Date;
		class DateTime;
	}
	 
	namespace env
	{

		class Path;
		class Calendar;


		/** Service abstract base class.

		A service represents the ability to follow a path
		at certain days and hours.

		The days when the service is provided are stored in a Calendar object.
		Even if a Service intrinsically corresponds to a sequence of 
		(arrival schedule - departure schedule) couples, these schedules are not
		stored in Service objects but per Edge (Line/Road). However, this is how 
		Service objects are persisted.

		It is completely independent from the "vehicle" : this ability 
		can be provided by an external entity (bus, train...), 
		but also self-provided by the traveller himself 
		(walking, cycling...).

		@ingroup m15
		*/
		class Service : 
			public BikeComplyer,
			public HandicappedComplyer,
			public PedestrianComplyer
		{
		private:
		    
			int				_serviceNumber;
			Calendar		_calendar;  //!< Which days is this service available ?
			Path*			_path;
			time::Schedule	_departureSchedule; //!< Service departure schedule (from the origin).
			time::Schedule	_arrivalSchedule;

		public:
			//! \name Update methods
			//@{

			//@}

			Service (int serviceNumber,
				Path* path,
				const synthese::time::Schedule& departureSchedule);
			Service();
			~Service ();

		    
			//! @name Getters
			//@{
				const Path*			getPath () const;
				Path*			getPath ();
				int				getServiceNumber () const;
				Calendar&		getCalendar (); // MJ constness pb

				/** Returns the departure schedule for this service.

					@return If this service is continuous, the first departure schedule
						(first course) is returned. Otherwise, it is the "normal" 
						departure schedule.
				*/
				const time::Schedule& getDepartureSchedule () const;

				/** Latest schedule of the service : the last arrival at the last vertex.
					@return The latest schedule of the service
				*/
				const time::Schedule& getLastArrivalSchedule() const;
			//@}

			//! @name Setters
			//@{
				void setPath(Path* path);
				void setServiceNumber (int serviceNumber);
				void setDepartureSchedule (const synthese::time::Schedule& departureSchedule);
				void setArrivalSchedule (const synthese::time::Schedule&);
			//@}



			//! @name Query methods
			//@{
				virtual bool isContinuous () const = 0;

				/** Accessor to the key of the service, provided by the implementation subclass.
					@return id of the service in the database
				*/
				virtual uid		getId()	const = 0;


				/** Is this service reservable ?
					@param departureMoment Desired departure moment
					@param calculationMoment Calculation moment taken as reference 
					for reservation delay calculation
					@return false in default implementation.
				*/
				virtual bool isReservationPossible ( const synthese::time::DateTime& departureMoment, 
								const synthese::time::DateTime& calculationMoment ) const;


				/** Is this service providen a given day ?
					@param departureDate Departure date of client
					@param jplus Number of days between client departure date 
					and departure date of the path service origin.
				*/
				bool isProvided(const synthese::time::Date& departureDate, int jplus ) const;

			//@}
		};
	}
}

#endif
