
//////////////////////////////////////////////////////////////////////////
/// VehiclePositionUpdateAction class header.
///	@file VehiclePositionUpdateAction.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_VehiclePositionUpdateAction_H__
#define SYNTHESE_VehiclePositionUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "VehiclePosition.hpp"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class ScheduledService;
	}

	namespace pt_operation
	{
		class Depot;
	}

	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		/// 38.15 Action : VehiclePositionUpdateAction.
		/// @ingroup m38Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : VehiclePositionUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class VehiclePositionUpdateAction:
			public util::FactorableTemplate<server::Action, VehiclePositionUpdateAction>
		{
		public:
			static const std::string PARAMETER_VEHICLE_ID;
			static const std::string PARAMETER_BEFORE_ID;
			static const std::string PARAMETER_VEHICLE_POSITION_ID;
			static const std::string PARAMETER_STATUS;
			static const std::string PARAMETER_TIME;
			static const std::string PARAMETER_METER_OFFSET;
			static const std::string PARAMETER_STOP_POINT_ID;
			static const std::string PARAMETER_COMMENT;
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_RANK_IN_PATH;
			static const std::string PARAMETER_PASSENGERS;
			static const std::string PARAMETER_SET_AS_CURRENT_POSITION;
			static const std::string PARAMETER_IN_STOP_AREA;
			static const std::string PARAMETER_STOP_FOUND_TIME;
			static const std::string PARAMETER_SET_NEXT_STOPS;

		private:
			boost::shared_ptr<VehiclePosition> _vehiclePosition;
			bool _setAsCurrentPosition;
			boost::optional<Vehicle*> _vehicle;
			boost::optional<VehiclePositionStatusEnum> _status;
			boost::optional<boost::posix_time::ptime> _time;
			boost::optional<VehiclePosition::Meters> _meterOffset;
			boost::optional<pt::StopPoint*> _stopPoint;
			boost::optional<pt_operation::Depot*> _depot;
			boost::optional<std::string> _comment;
			boost::optional<pt::ScheduledService*> _service;
			boost::optional<boost::optional<std::size_t> > _rankInPath;
			boost::optional<std::size_t> _passengers;
			boost::optional<bool> _inStopArea;
			boost::optional<boost::posix_time::ptime> _nextStopFoundTime;
			bool _setNextStops;

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
			VehiclePositionUpdateAction();

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
				void setVehiclePosition(boost::shared_ptr<VehiclePosition> value) { _vehiclePosition = value; }
				void setVehicle(Vehicle* value) { _vehicle = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_VehiclePositionUpdateAction_H__
