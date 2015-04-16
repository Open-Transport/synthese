
//////////////////////////////////////////////////////////////////////////
/// LineStopAddAction class header.
///	@file LineStopAddAction.h
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_LineStopAddAction_H__
#define SYNTHESE_LineStopAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include "ContinuousServiceTableSync.h"
#include "ScheduledServiceTableSync.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
		class StopPoint;
		class DRTArea;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Adds a stop into a route.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : LineStopAddAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the line</li>
		///		<li>actionParamrk : rank where insert the stop</li>
		///		<li>actionParamcn : name of the city of the stop</li>
		///		<li>actionParamsn : name of the stop</li>
		///		<li>actionParammo : metric offset</li>
		///	</ul>
		///
		/// TODO : If the metric offset is undefined by the parameters, a default value is
		/// generated :
		///	<ul>
		///		<li>by adding the distance between points by direct line to the metric offset
		///		of the previous stop, if the coordinates are known and if there is a previous stop</li>
		///		<li>by dividing by 2 the distance between the next and the previous stop if there is
		///		a next and a previous stop</li>
		///		<li>by adding 500 meters to the preceding stop if there is a previous stop</li>
		///		<li>0 else</li>
		///	</ul>
		class LineStopAddAction:
			public util::FactorableTemplate<server::Action, LineStopAddAction>
		{
		public:
			static const std::string PARAMETER_ROUTE_ID;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_STOP_NAME;
			static const std::string PARAMETER_METRIC_OFFSET;
			static const std::string PARAMETER_AREA;
			static const std::string PARAMETER_WITH_SCHEDULES;
			static const std::string PARAMETER_RESERVATION_NEEDED;
			static const std::string PARAMETER_DURATION_TO_ADD;

		private:
			boost::shared_ptr<JourneyPattern> _route;
			ScheduledServiceTableSync::SearchResult _scheduledServices;
			ContinuousServiceTableSync::SearchResult _continuousServices;
			boost::shared_ptr<StopPoint> _stop;
			boost::shared_ptr<DRTArea> _area;
			std::size_t _rank;
			double _metricOffset;
			bool _withSchedules;
			bool _reservationNeeded;
			boost::posix_time::time_duration _durationToAdd;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			LineStopAddAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setRoute(boost::shared_ptr<JourneyPattern> value) { _route = value; }
				void setRank(std::size_t value){ _rank = value; }
				void setWithSchedules(bool value){ _withSchedules = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_LineStopAddAction_H__
