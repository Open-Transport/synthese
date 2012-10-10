
/** RealTimeVehicleUpdateAction class header.
	@file RealTimeVehicleUpdateAction.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_RealTimeVehicleUpdateAction_H__
#define SYNTHESE_RealTimeVehicleUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ServicePointer.h"
#include <boost/date_time/time_duration.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace pt
	{
		class ScheduledService;
	}

	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		///	Action class : real time update of service schedules.
		///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m38Actions refActions
		class RealTimeVehicleUpdateAction:
			public util::FactorableTemplate<server::Action, RealTimeVehicleUpdateAction>
		{
		public:
			static const std::string PARAMETER_VEHICLE_SERVICE_DATASOURCE_ID;
			static const std::string PARAMETER_VEHICLE_SERVICE_ID;
			static const std::string PARAMETER_VEHICLE_SERVICE_SERVICE_RANK;
			static const std::string PARAMETER_VEHICLE_SERVICE_SERVICE_OR_DRIVER_EXCHANGE_RANK;
			static const std::string PARAMETER_LATE_DURATION_SECONDS;

			static const std::string PARAMETER_LINE_STOP_RANK;
			static const std::string PARAMETER_LINE_STOP_METRIC_OFFSET;
			static const std::string PARAMETER_AT_ARRIVAL;
			static const std::string PARAMETER_AT_DEPARTURE;
			static const std::string PARAMETER_PROPAGATE_CONSTANTLY;
			static const std::string PARAMETER_DEPARTURE_TIME;
			static const std::string PARAMETER_ARRIVAL_TIME;

		private:
			pt::ScheduledService* _service;
			boost::shared_ptr<impex::DataSource> _dataSource;
			std::size_t _lineStopRank;
			boost::posix_time::time_duration _lateDuration;
			bool _atArrival;
			bool _atDeparture;
			bool _propagateConstantly;
			boost::posix_time::time_duration _departureTime;
			boost::posix_time::time_duration _arrivalTime;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	See https://extranet.rcsmobility.com/projects/synthese/wiki/Real_time_update_of_scheduled_service#Request
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			RealTimeVehicleUpdateAction();

			//////////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			/// @todo Implement journalization
			void run(server::Request& request);

			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// @todo Implement a right control on this function
			virtual bool isAuthorized(const server::Session* session) const;

			virtual bool _isSessionRequired() const { return false; }

			//! @name Setters
			//@{
				void setService(const pt::ScheduledService* service);
				void setLineStopRank(std::size_t value);
				void setAtArrival(bool value);
				void setAtDeparture(bool value);
				void setPropagateConstantly(bool value);
				void setDelay(boost::posix_time::time_duration value);
			//@}
		};
	}
}

#endif // SYNTHESE_RealTimeVehicleUpdateAction_H__
