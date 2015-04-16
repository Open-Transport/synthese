
/** AlgorithmLogger class header.
	@file AlgorithmLogger.hpp

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

#ifndef SYNTHESE_algorithm_AlgorithmLogger_hpp__
#define SYNTHESE_algorithm_AlgorithmLogger_hpp__

#include "AlgorithmTypes.h"
#include "HTMLTable.h"
#include "JourneysResult.h"

#include <fstream>
#include <vector>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/filesystem/path.hpp>

namespace synthese
{
	namespace graph
	{
		class Vertex;
	}

	namespace algorithm
	{
		class RoutePlanningIntermediateJourney;

		/** AlgorithmLogger class.
			@ingroup m33
		*/
		class AlgorithmLogger
		{
		private:
			const boost::filesystem::path _directory;
			bool _active;

			boost::shared_ptr<std::ofstream> _openNewFile() const;
			mutable size_t _fileNumber;
			mutable boost::posix_time::time_duration _chrono;
			mutable boost::posix_time::ptime _chronoStartTime;
			void _startChrono() const;
			boost::posix_time::time_duration _stopChrono() const;

			boost::filesystem::path _getCurrentFilePath() const;

			/// @name Integral search
			//@{
				mutable boost::shared_ptr<std::ofstream> _integralSearchFile;
				mutable boost::posix_time::ptime _integralSearchDesiredTime;
				mutable PlanningPhase _integralSearchPlanningPhase;
			//@}

			/// @name Journey planner step
			//@{
				mutable boost::shared_ptr<std::ofstream> _journeyPlannerStepFile;
				mutable html::HTMLTable _journeyPlannerStepTable;
				mutable std::size_t _journeyPlannerSearchNumber;
				mutable boost::posix_time::time_duration _journeyPlannerStepStartChrono;
			//@}

			/// @name Journey planner
			//@{
				mutable boost::shared_ptr<std::ofstream> _journeyPlannerFile;
				typedef std::map<const graph::Vertex*, boost::shared_ptr<RoutePlanningIntermediateJourney> > Map;
				typedef std::vector<boost::shared_ptr<RoutePlanningIntermediateJourney> > Vector;
				mutable boost::posix_time::time_duration _journeyPlannerStartChrono;
				mutable boost::posix_time::ptime _journeyPlannerOriginDateTime;
				mutable html::HTMLTable _journeyPlannerTable;
				mutable const RoutePlanningIntermediateJourney* _journeyPlannerResult;
				mutable Map _lastTodo;
				mutable Vector _todoBeforeClean;
				mutable PlanningPhase _journeyPlanningPhase;
				mutable size_t _timeSlotJourneyPlannerStepNumber;
			//@}

			/// @name Time slot journey planner
			//@{
				mutable boost::shared_ptr<std::ofstream> _timeSlotJourneyPlannerFile;
				mutable html::HTMLTable _timeSlotJourneyPlannerTable;
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			AlgorithmLogger(
				boost::filesystem::path dir = boost::filesystem::path()
			);

			/// @name Getters
			//@{
				const boost::filesystem::path& getDirectory() const { return _directory; }
			//@}

			/// @name Integral search
			//@{
				void openIntegralSearchLog(
					PlanningPhase planningPhase,
					const boost::posix_time::ptime& desiredTime
				) const;

				void logIntegralSearchJourney(
					const RoutePlanningIntermediateJourney& journey
				) const;

				void closeIntegralSearchLog(
				) const;
			//@}

			/// @name Journey planner
			//@{
				void openJourneyPlannerLog(
					const boost::posix_time::ptime& originDateTime,
					PlanningPhase planningPhase
				) const;
				void recordJourneyPlannerLogIntegralSearch(
					boost::shared_ptr<const RoutePlanningIntermediateJourney> journey,
					const boost::posix_time::ptime& bestDateTime,
					const JourneysResult& todo
				) const;
				void recordJourneyPlannerLogCleanup(
					bool resultFound,
					const boost::posix_time::ptime& bestDateTime,
					const JourneysResult& todo
				) const;
				void closeJourneyPlannerLog() const;
			//@}

			/// @name Time slot journey planner
			//@{
				void openTimeSlotJourneyPlannerLog() const;
				void logTimeSlotJourneyPlannerStep(
					const boost::posix_time::ptime& originDateTime
				) const;
				void logTimeSlotJourneyPlannerApproachMap(
					bool isDeparture,
					const graph::VertexAccessMap& vam
				) const;
				void closeTimeSlotJourneyPlannerLog() const;
			//@}
		};
	}
}

#endif // SYNTHESE_algorithm_AlgorithmLogger_hpp__

