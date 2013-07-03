
/** SchedulesBasedService class header.
	@file SchedulesBasedService.h

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

#ifndef SYNTHESE_pt_SchedulesBasedService_h__
#define SYNTHESE_pt_SchedulesBasedService_h__

#include "NonPermanentService.h"
#include "JourneyPattern.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace pt
	{
		/** SchedulesBasedService class.
			@ingroup m35
		*/
		class SchedulesBasedService:
			public NonPermanentService
		{
		public:
			typedef std::vector<boost::posix_time::time_duration> Schedules;
			typedef std::vector<boost::posix_time::ptime> TimestampSchedules;

			class BadSchedulesException: public synthese::Exception
			{
			public:
				BadSchedulesException(): Exception("Bad schedule") {}
			};

			class PathBeginsWithUnscheduledStopException: public synthese::Exception
			{
			public:
				PathBeginsWithUnscheduledStopException(const graph::Path& path);
			};

			typedef std::vector<const graph::Vertex*> ServedVertices;

			static const std::string STOP_SEPARATOR;

		private:
			typedef std::map<
				boost::tuples::tuple<
					const graph::Edge*,
					const graph::Edge*,
					std::size_t,
					boost::gregorian::date
				>,
			    std::vector<boost::posix_time::time_period>
			> _NonConcurrencyCache;

			mutable _NonConcurrencyCache _nonConcurrencyCache;
			mutable boost::recursive_mutex _nonConcurrencyCacheMutex;

			bool isInTimeRange(boost::posix_time::ptime &time,
					boost::posix_time::time_duration &range, 
					const std::vector<boost::posix_time::time_period> &excludeRanges
			) const;

			// This is the schedule as we got it when loaded from the base
			std::string _rawSchedule;

		protected:

			//! @name Theoretical data
			//@{
				Schedules	_departureSchedules;	//!< Departure schedules
				Schedules	_arrivalSchedules;		//!< Arrival schedules
				ServedVertices	_vertices;			//!< Edges
			//@}

			//! @name Real time data
			//@{
				Schedules	_RTDepartureSchedules;
				Schedules	_RTArrivalSchedules;
				ServedVertices	_RTVertices;		//!< Real time edges
				boost::posix_time::ptime _nextRTUpdate;
				TimestampSchedules _RTTimestamps; 	//!< Hold the time stamp of updates
				Schedules	_emptySchedules;	//!< Always empty departure schedules
				bool _hasRealTimeData;
			//@}

			void _applyRealTimeShiftDuration(
				std::size_t rank,
				boost::posix_time::time_duration arrivalShift,
				boost::posix_time::time_duration departureShift,
				bool updateFollowingSchedules
			);
		public:
			SchedulesBasedService(
				std::string serviceNumber,
				graph::Path* path
			);


			//! @name Getters
			//@{
				const Schedules& getDepartureSchedules(bool THData, bool RTData) const;
				const Schedules& getArrivalSchedules(bool THData, bool RTData) const;
				const ServedVertices& getVertices(bool RTData) const;
				const boost::posix_time::ptime& getNextRTUpdate() const;
			//@}

			//! @name Setters
			//@{

				/** Schedules update.
					Updates both theoretical and real time data.
					@param departureSchedules Departure schedules of the service
					@param arrivalSchedules Arrival schedules of the service
					@param onlyScheduledEdges Only schedules at scheduled edges are present in the data to load. The missing schedules will be interpolated.
					@author Hugues Romain
				*/
				void setSchedules(
					const Schedules& departureSchedules,
					const Schedules& arrivalSchedules,
					bool onlyScheduledEdges
				);


				virtual void setPath(graph::Path* path);
			//@}


				const graph::Vertex* getRealTimeVertex(
					std::size_t rank
				) const;


				const graph::Vertex* getVertex(
					std::size_t rank
				) const;




				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return The schedule at the specified stop rank
				*/
				virtual boost::posix_time::time_duration getDepartureSchedule (bool RTData, std::size_t rank) const;

				const boost::posix_time::time_duration& getLastDepartureSchedule(bool RTData) const;

				virtual boost::posix_time::time_duration getArrivalSchedule (bool RTData, std::size_t rank) const;

				virtual const boost::posix_time::time_duration& getLastArrivalSchedule(bool RTData) const;

				graph::Edge* getEdgeFromStopAndTime(
					const graph::Vertex& vertex,
					const boost::posix_time::time_duration& schedule,
					bool departure
				) const;

			//! @name Update methods
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Apply a shift duration on real time schedules of the service.
				/// @param rank Rank of the first edge where the shift begins.
				///		rank must be inferior than the size of the schedules array
				/// @param arrivalShift the time shift on arrival
				/// @param departureShift the time shift on departure
				/// @param updateFollowingSchedules true if the method must propagate the
				///		shift on each following edges (at arrivals and departures)
				/// @param recordTimeStamp 
				void applyRealTimeShiftDuration(
					std::size_t rank,
					boost::posix_time::time_duration arrivalShift,
					boost::posix_time::time_duration departureShift,
					bool updateFollowingSchedules,
					bool recordTimeStamp = true
				);

				void setRealTimeSchedules(
					std::size_t rank,
					boost::posix_time::time_duration departureSchedule,
					boost::posix_time::time_duration arrivalSchedule
				);

				void setRealTimeSchedules(
					const Schedules& departureSchedules,
					const Schedules& arrivalSchedules
				);


				void _computeNextRTUpdate();

				void setSchedulesFromOther(const SchedulesBasedService& other, boost::posix_time::time_duration shift);
				void generateIncrementalSchedules(boost::posix_time::time_duration firstSchedule);

				bool comparePlannedSchedules(const Schedules& departure, const Schedules& arrival) const;
				bool comparePlannedStops(const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedVertices) const;


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



				void setRealTimeVertices(
					const ServedVertices& value
				);


				//////////////////////////////////////////////////////////////////////////
				/// Restores real time data to theoretical value.
				/// Sets the next update into the next day.
				virtual void clearRTData();


				//////////////////////////////////////////////////////////////////////////
				/// Clears stops data.
				void clearStops();
			//@}

			//! @name Encoding / decoding
			//@{
				static std::string EncodeSchedule(const boost::posix_time::time_duration& value);
				static boost::posix_time::time_duration DecodeSchedule(const std::string value);


				//////////////////////////////////////////////////////////////////////////
				/// Encode schedules into a string.
				/// Schedules at non scheduled stops are ignored.
				/// @param shiftArrivals duration to add to the arrival times before encoding (default 0)
				/// @author Hugues Romain
				std::string encodeSchedules(
					boost::posix_time::time_duration shiftArrivals = boost::posix_time::minutes(0)
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads schedules from encoded strings.
				/// Schedules at non scheduled stops are ignored.
				/// @param value encoded strings
				/// @param shiftArrivals duration to add to the arrival times (default 0)
				/// @author Hugues Romain
				void decodeSchedules(
					const std::string value,
					boost::posix_time::time_duration shiftArrivals = boost::posix_time::minutes(0)
				);


				std::string encodeStops() const;
				void decodeStops(const std::string& value, util::Env& env);


				void setVertex(
					size_t rank,
					const graph::Vertex* value
				);



				void setVertices(
					const ServedVertices& vertices
				);

			//@}

			//! @name Query methods
			//@{
				/**
				 * @brief nonConcurrencyRuleOK
				 * @param time[in,out] the date to check. it may be moved later to a valid start date.
				 * @param range[in,out] the range to check. it may be changed if we are
				 * a continuous service and got a non concurrency rule at some point in time
				 * @param departureEdge
				 * @param arrivalEdge
				 * @param userClassRank
				 * @return 
				 */
				virtual bool nonConcurrencyRuleOK(
					boost::posix_time::ptime& time,
					boost::posix_time::time_duration &range,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					std::size_t userClassRank
				) const;

				virtual void clearNonConcurrencyCache() const;
			//@}
				
		};

		bool operator==(const SchedulesBasedService& first, const SchedulesBasedService& second);

	}
}

#endif // SYNTHESE_pt_SchedulesBasedService_h__
