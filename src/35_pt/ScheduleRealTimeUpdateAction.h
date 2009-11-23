
/** ScheduleRealTimeUpdateAction class header.
	@file ScheduleRealTimeUpdateAction.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_ScheduleRealTimeUpdateAction_H__
#define SYNTHESE_ScheduleRealTimeUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ServicePointer.h"
#include <boost/date_time/time_duration.hpp>

namespace synthese
{
	namespace env
	{
		class ScheduledService;
	}

	namespace pt
	{
		/** Action class : real time update of service schedules.
			@ingroup m35Actions refActions

			Parameters :
				- se : service ID
				- ls : line stop ID
				- la : late duration (minutes)
				- aa : the late concerns the arrival time at the edge (default=true)
				- ad : the late concerns the departure time from the edge (default=true)
				- pc : the late must be propagated into the following edges with the same value (default=true)
		*/
		class ScheduleRealTimeUpdateAction:
			public util::FactorableTemplate<server::Action, ScheduleRealTimeUpdateAction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_LINE_STOP_RANK;
			static const std::string PARAMETER_LATE_DURATION_MINUTES;
			static const std::string PARAMETER_AT_ARRIVAL;
			static const std::string PARAMETER_AT_DEPARTURE;
			static const std::string PARAMETER_PROPAGATE_CONSTANTLY;

		private:
			boost::shared_ptr<env::ScheduledService> _service;
			std::size_t _lineStopRank;
			boost::posix_time::time_duration _lateDuration;
			bool _atArrival;
			bool _atDeparture;
			bool _propagateConstantly;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			/// @todo Implement journalization
			void run();

			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// @todo Implement a right control on this function
			virtual bool _isAuthorized() const;

			//! @name Setters
			//@{
				void setService(boost::shared_ptr<const env::ScheduledService> service);
				void setLineStopRank(std::size_t value);
				void setAtArrival(bool value);
				void setAtDeparture(bool value);
				void setPropagateConstantly(bool value);
				void setDelay(boost::posix_time::time_duration value);
			//@}
		};
	}
}

#endif // SYNTHESE_ScheduleRealTimeUpdateAction_H__
