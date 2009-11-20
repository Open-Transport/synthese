
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

#include "ServicePointer.h"
#include "Schedule.h"
#include "Registrable.h"
#include "Registry.h"
#include "RuleUser.h"

#include <string>

namespace synthese
{
	namespace time
	{
		class Date;
		class DateTime;
	}

	namespace graph
	{
		class Path;

		/** Service abstract base class.
			TRIDENT = VehicleJourney

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

			@ingroup m18
		*/
		class Service
		:	public RuleUser,
			public virtual util::Registrable		
		{
		public:

		private:
			std::string				_serviceNumber;
			util::RegistryKeyType	_pathId;
			Path*					_path;
			
		protected:
			virtual const RuleUser* _getParentRuleUser() const;

		public:

			Service(
				const std::string& serviceNumber,
				Path* path
			);

			Service(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);
			~Service ();


			//! @name Getters
			//@{
				const Path*			getPath () const;
				Path*				getPath ();
				uid					getPathId()			const;
				const std::string&	getServiceNumber()	const;


				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return see the implementations of the method.
				*/
				virtual time::Schedule getDepartureSchedule(
					bool RTData,
					size_t rank
				) const = 0;
			//@}

			//! @name Setters
			//@{
				void setPath(Path* path);
				void setPathId(uid id);
				void setServiceNumber(std::string serviceNumber);
			//@}



			//! @name Query methods
			//@{
				virtual bool isContinuous () const = 0;



				virtual std::string getTeam() const;

				virtual time::Schedule getDepartureBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;
				virtual time::Schedule getDepartureEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;
				virtual time::Schedule getArrivalBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;
				virtual time::Schedule getArrivalEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual bool nonConcurrencyRuleOK(
					const time::Date& date,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					graph::UserClassCode userClass
				) const;
				
				virtual void clearNonConcurrencyCache() const;


				/** Tests if the service could be inserted in the same line than the current one, according to the line theory.
					@param other service to test
					@return bool true if the two services are compatible
					@author Hugues Romain
					@date 2008
					@warning the method must be used only at a time where the service knows its path, so use the method on the currently registered service, taking the new one as parameter
				*/
				bool respectsLineTheoryWith(
					bool RTData,
					const Service& other
				) const;



				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param method Search departure or arrival :
						- ServicePointer::DEPARTURE_TO_ARRIVAL
						- ServicePointer::ARRIVAL_TO_DEPARTURE
					@param edge Edge
					@param presenceDateTime Goal  time
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param inverted : indicates if the range computing must follow the same rules as method says (false) or the inverted ones (true)
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007
					@warning The service index is unknown in the generated ServicePointer.					
				*/
				virtual ServicePointer getFromPresenceTime(
					bool RTData,
					AccessDirection method,
					UserClassCode userClass
					, const Edge* edge
					, const time::DateTime& presenceDateTime
					, bool controlIfTheServiceIsReachable
					, bool inverted
				) const = 0;

				virtual time::DateTime getLeaveTime(
					const ServicePointer& servicePointer
					, const Edge* edge
				) const = 0;

				/** Date of the departure from the origin of the service.
				@param departureDate Date of use of the service at the scheduled point
				@param departureTime Known schedule of departure in the service journey
				@return time::DateTime Date of the departure from the origin of the service.
				@author Hugues Romain
				@date 2007

				*/
				time::DateTime getOriginDateTime(
					bool RTData,
					const time::Date& departureDate,
					const time::Schedule& departureTime
				) const;
			//@}
		};
	}
}

#endif
