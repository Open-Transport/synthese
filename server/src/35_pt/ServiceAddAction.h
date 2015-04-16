
//////////////////////////////////////////////////////////////////////////
/// ServiceAddAction class header.
///	@file ServiceAddAction.h
///	@author Hugues
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

#ifndef SYNTHESE_ServiceAddAction_H__
#define SYNTHESE_ServiceAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
	}

	namespace pt
	{
		class ContinuousService;
		class ScheduledService;
		class SchedulesBasedService;

		//////////////////////////////////////////////////////////////////////////
		/// ServiceAddAction action class.
		/// @ingroup m35Actions refActions
		class ServiceAddAction:
			public util::FactorableTemplate<server::Action, ServiceAddAction>
		{
		public:
			static const std::string PARAMETER_IS_CONTINUOUS;
			static const std::string PARAMETER_LINE_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_START_DEPARTURE_TIME;
			static const std::string PARAMETER_END_DEPARTURE_TIME;
			static const std::string PARAMETER_PERIOD;

		private:
			bool _isContinuous;
			boost::shared_ptr<pt::JourneyPattern> _line;
			boost::shared_ptr<const SchedulesBasedService> _template;
			boost::posix_time::time_duration _period;
			std::string _number;
			boost::posix_time::time_duration _startDepartureTime;
			boost::posix_time::time_duration _endDepartureTime;




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

			ServiceAddAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			void setIsContinuous(bool value);
			void setLine(boost::shared_ptr<pt::JourneyPattern> value);
		};
	}
}

#endif // SYNTHESE_ServiceAddAction_H__
