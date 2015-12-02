
//////////////////////////////////////////////////////////////////////////
/// ServiceTimetableUpdateAction class header.
///	@file ServiceTimetableUpdateAction.h
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

#ifndef SYNTHESE_ServiceTimetableUpdateAction_H__
#define SYNTHESE_ServiceTimetableUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Service schedules update.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		///	There is two ways to update the schedules of a service :
		///	<ul>
		///		<li>by typing  the timetable</li>
		///		<li>by introducting a shifting delay</li>
		///	</ul>
		///
		/// Key : ServiceTimetableUpdateAction
		///
		///	<h2>Timetable typing</h2>
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the service to update</li>
		///		<li>actionParamrk : rank of the stop in the service</li>
		///		<li>actionParamar : 1 for update of arrival time, 0 for departure time</li>
		///		<li>actionParamti : new value for time (iso format, hours > 24 if necessary)</li>
		///	</ul>
		///
		///	<h2>Shifting delay</h2>
		///
		/// With shifting delay, all stops after the specified one are updated too.
		/// If shifting the arrival time, the departure time is shifted too.
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the service to update</li>
		///		<li>actionParamrk : rank of the stop in the service</li>
		///		<li>actionParamar : 1 for update of arrival time, 0 for departure time</li>
		///		<li>actionParamsd : Shifting delay (minutes), can be negative</li>
		///	</ul>
		///
		class ServiceTimetableUpdateAction:
			public util::FactorableTemplate<server::Action, ServiceTimetableUpdateAction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_UPDATE_ARRIVAL;
			static const std::string PARAMETER_TIME;
			static const std::string PARAMETER_COMMENT;
			static const std::string PARAMETER_SHIFTING_DELAY;

		private:
			boost::shared_ptr<SchedulesBasedService> _service;
			std::size_t _rank;
			bool _updateArrival;
			boost::posix_time::time_duration _time;
			boost::posix_time::time_duration _shifting_delay;
			boost::optional<std::string> _comment;

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
			ServiceTimetableUpdateAction();

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
				void setService(boost::shared_ptr<SchedulesBasedService> value) { _service = value; }
				void setRank(std::size_t value) { _rank=value; }
				void setUpdateArrival(bool value) { _updateArrival=value; }
			//@}
		};
	}
}

#endif // SYNTHESE_ServiceTimetableUpdateAction_H__
