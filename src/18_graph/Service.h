
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
#include "Registrable.h"
#include "Registry.h"
#include "RuleUser.h"

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace graph
	{
		class Path;
		class AccessParameters;

		/** Service abstract base class.

			A service represents the ability to follow a path
			at certain days and hours.

			The days when the service is provided are stored in a Calendar object.
			Even if a Service intrinsically corresponds to a sequence of
			(arrival schedule - departure schedule) couples, these schedules are not
			stored in Service objects but per Edge (JourneyPattern/Road). However, this is how
			Service objects are persisted.

			It is completely independent from the "vehicle" : this ability
			can be provided by an external entity (bus, train...),
			but also self-provided by the traveller himself
			(walking, cycling...).

			@warning Each constructor of Service implementations must run clearRTData().


			@ingroup m18
		*/
		class Service
		:	public RuleUser,
			public virtual util::Registrable
		{
		public:
			typedef std::vector<const Vertex*> ServedVertices;
			static const boost::posix_time::time_duration DAY_DURATION;

		protected:
			std::string				_serviceNumber;
			util::RegistryKeyType	_pathId;
			Path*					_path;

			boost::posix_time::ptime _nextRTUpdate;
			ServedVertices	_RTVertices;	//!< Real time edges

			virtual void _computeNextRTUpdate() = 0;

		public:

			Service(
				const std::string& serviceNumber,
				Path* path
			);

			Service(
				util::RegistryKeyType id = 0
			);
			~Service ();


			//! @name Getters
			//@{
				const Path*			getPath () const;
				Path*				getPath ();
				util::RegistryKeyType		getPathId()			const { return _pathId; }
				const std::string&	getServiceNumber()	const;
				const boost::posix_time::ptime& getNextRTUpdate() const;
			//@}

			//! @name Setters
			//@{
				virtual void setPath(Path* path);
				void setPathId(util::RegistryKeyType id) { _pathId = id; }
				void setServiceNumber(std::string serviceNumber);
			//@}



			//! @name Services
			//@{
				virtual const RuleUser* _getParentRuleUser() const;

				virtual std::string getRuleUserName() const { return "Service " + getServiceNumber(); }

				virtual bool isContinuous () const = 0;



				virtual std::string getTeam() const;

				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual bool nonConcurrencyRuleOK(
					const boost::gregorian::date& date,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					std::size_t userClassRank
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
					@param getDeparture
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
					bool getDeparture,
					std::size_t userClassRank,
					const Edge& edge,
					const boost::posix_time::ptime& presenceDateTime,
					bool controlIfTheServiceIsReachable,
					bool inverted,
					bool ignoreReservation
				) const = 0;



				virtual void completeServicePointer(
					ServicePointer& servicePointer,
					const Edge& edge,
					const AccessParameters& accessParameters
				) const = 0;

				/** Date of the departure from the origin of the service.
				@param departureDate Date of use of the service at the scheduled point
				@param departureTime Known schedule of departure in the service journey
				@return Date of the departure from the origin of the service.
				@author Hugues Romain
				@date 2007

				*/
				boost::posix_time::ptime getOriginDateTime(
					bool RTData,
					const boost::gregorian::date& departureDate,
					const boost::posix_time::time_duration& departureTime
				) const;

				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return see the implementations of the method.
				*/
				virtual boost::posix_time::time_duration getDepartureSchedule(
					bool RTData,
					size_t rank
				) const = 0;


				const graph::Vertex* getRealTimeVertex(
					std::size_t rank
				) const;
			//@}




			//! @name Update methods
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Update a served edge at real time.
				/// @param rank Rank of the edge to update
				/// @param value Served edge
				void setRealTimeVertex(
					std::size_t rank,
					const graph::Vertex* value
				);


				//////////////////////////////////////////////////////////////////////////
				/// Restores real time data to theoretical value.
				/// Sets the next update into the next day.
				virtual void clearRTData();
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Gets the time of day of a duration which can be greater than 24 hours
			/// Example : 25:00:00 => 1:00:00
			/// @param value the duration to transform
			/// @result the time of day duration
			static boost::posix_time::time_duration GetTimeOfDay(const boost::posix_time::time_duration& value);
		};
	}
}

#endif
