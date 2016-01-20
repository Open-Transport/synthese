
/** ServicePointer class header.
	@file ServicePointer.h

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

#ifndef SYNTHESE_env_ServicePointer_h__
#define SYNTHESE_env_ServicePointer_h__

#include "GraphTypes.h"
#include "UseRule.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace geos
{
	namespace geom
	{
		class LineString;
	}
}

namespace synthese
{
	namespace graph
	{
		class Service;
		class Edge;
		class Vertex;

		/** Service Pointer class.
			@ingroup m18
			@author Hugues Romain

			A service pointer determines :
				- a service
				- a calendar date
				- an edge
				- a direction (departure from the edge or arrival at the edge)

			If the departure edge is null, then all the informations about the departure are not known at the moment.
			If the arrival edge is null, then all the informations about the arrival are not known at the moment.
		*/
		class ServicePointer
		{
		protected:
			//! @name Departure informations
			//@{
				const Edge*			_departureEdge;
				boost::posix_time::ptime	_departureTime;
				const Vertex*		_realTimeDepartureVertex;
				boost::posix_time::ptime		_theoreticalDepartureTime;
			//@}

			//! @name Arrival informations
			//@{
				const Edge*			_arrivalEdge;
				boost::posix_time::ptime	_arrivalTime;
				const Vertex*		_realTimeArrivalVertex;
				boost::posix_time::ptime		_theoreticalArrivalTime;
			//@}

			//! @name General information
			//@{
				std::size_t		_userClassRank;
				bool				_THData;
				bool				_RTData;
				const Service*	_service;
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::time_duration	_range;
				bool			_canceled;
				boost::shared_ptr<geos::geom::LineString> _customGeometry;
			//@}

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			ServicePointer(
				bool THData,
				bool RTData,
				std::size_t userClassRank,
				const Service& service,
				const boost::posix_time::ptime& originDateTime
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			ServicePointer(
				bool THData,
				bool RTData,
				std::size_t userClassRank,
				const Service& service,
				const boost::gregorian::date& date,
				const Edge& departureEdge,
				const Edge& arrivalEdge
			);



			//////////////////////////////////////////////////////////////////////////
			/// @pre partially filled pointer must not be completed.
			ServicePointer(
				const ServicePointer& partiallyFilledPointer,
				const Edge& edge,
				const AccessParameters& accessParameters
			);



			//////////////////////////////////////////////////////////////////////////
			/// Null service pointer.
			ServicePointer();

			//! @name Setters
			//@{
				void	setServiceRange(boost::posix_time::time_duration duration){ _range = duration; }
				void setUserClassRank( std::size_t value) { _userClassRank = value; }
			//@}

			//! @name Getters
			//@{
				const Edge*				getDepartureEdge()	const { return _departureEdge; }
				const Edge*				getArrivalEdge()	const { return _arrivalEdge; }
				const Vertex*			getRealTimeDepartureVertex()			const { return _realTimeDepartureVertex; }
				const Vertex*			getRealTimeArrivalVertex()			const { return _realTimeArrivalVertex; }
				const Service*			getService()				const { return _service; }
				const boost::posix_time::ptime&	getDepartureDateTime()			const { return _departureTime; }
				const boost::posix_time::ptime&	getArrivalDateTime()			const { return _arrivalTime; }
				const boost::posix_time::ptime&	getTheoreticalDepartureDateTime()	const { return _theoreticalDepartureTime; }
				const boost::posix_time::ptime&	getTheoreticalArrivalDateTime()	const { return _theoreticalArrivalTime; }
				const boost::posix_time::ptime&	getOriginDateTime()			const { return _originDateTime; }
				boost::posix_time::time_duration	getServiceRange() const { return _range; }
				std::size_t				getUserClassRank() const { return _userClassRank; }
				bool					getTHData() const { return _THData; }
				bool					getRTData() const { return _RTData; }
				bool					getCanceled() const { return _canceled; }
			//@}

			//! @name Update methods
			//@{
				void	setDepartureInformations(
					const graph::Edge& edge,
					const boost::posix_time::ptime& dateTime,
					const boost::posix_time::ptime& theoreticalDateTime,
					const Vertex& realTimeVertex
				);
				void	setArrivalInformations(
					const graph::Edge& edge,
					const boost::posix_time::ptime& dateTime,
					const boost::posix_time::ptime& theoreticalDateTime,
					const Vertex& realTimeVertex
				);

				//////////////////////////////////////////////////////////////////////////
				// Sets departure informations for a canceled service
				void	setDepartureInformations(
					const graph::Edge& edge,
					const boost::posix_time::ptime& dateTime,
					const boost::posix_time::ptime& theoreticalDateTime
				);

				//////////////////////////////////////////////////////////////////////////
				// Sets arrival informations for a canceled service
				void	setArrivalInformations(
					const graph::Edge& edge,
					const boost::posix_time::ptime& dateTime,
					const boost::posix_time::ptime& theoreticalDateTime
				);

				void shift(boost::posix_time::time_duration duration);

				void setCustomGeometry(boost::shared_ptr<geos::geom::LineString> geometry);
			//@}

			//! @name Queries
			//@{
				bool operator==(const ServicePointer& rhs) const;
				bool operator!=(const ServicePointer& rhs) const;


				bool hysteresisCompare(
					const ServicePointer& rhs,
					const boost::posix_time::time_duration& hysteresis
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// @pre both departure edge and arrival edge must be defined
				double getDistance() const;


				boost::posix_time::time_duration getDuration() const;

				/** Test the respect of the reservation rules.
					@return bool true if the service can be used
					@warning If the service is determined by ARRIVAL_TO_DEPARTURE, then this method always aswers true, because the reservation
						deadline depends only on the departure time, which is not known at this stage. Use ServiceUse::isReservationRuleCompliant method to validate the reservation
						deadline respect.
					@author Hugues Romain
					@date 2007
				*/
				UseRule::RunPossibilityType isUseRuleCompliant(
					bool ignoreReservation,
					UseRule::ReservationDelayType reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY
				);


				//////////////////////////////////////////////////////////////////////////
				/// @pre departure informations must be set
				boost::posix_time::ptime getReservationDeadLine(
					UseRule::ReservationDelayType reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Gets the use rule applicable to the user registered in the service use.
				/// @return the use rule
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				const UseRule& getUseRule() const;


				//////////////////////////////////////////////////////////////////////////
				/// Extracts from the edges the geometry of the whole journey.
				/// @pre the departure edge and the arrival edge must be defined
				/// @return the geometry
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				boost::shared_ptr<geos::geom::LineString> getGeometry() const;
			//@}
		};
	}
}

#endif // SYNTHESE_env_ServicePointer_h__
