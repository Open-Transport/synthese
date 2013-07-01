
//////////////////////////////////////////////////////////////////////////
/// JourneyPatternUpdateAction class header.
///	@file JourneyPatternUpdateAction.hpp
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

#ifndef SYNTHESE_LineUpdateAction_H__
#define SYNTHESE_LineUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ImportableTemplate.hpp"
#include "BaseImportableUpdateAction.hpp"
#include "BaseCalendarUpdateAction.hpp"

#include "GraphTypes.h"

namespace synthese
{
	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt
	{
		class CommercialLine;
		class JourneyPattern;
		class Destination;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Route properties update.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_pattern_update
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		class JourneyPatternUpdateAction:
			public util::FactorableTemplate<server::Action, JourneyPatternUpdateAction>,
			public impex::BaseImportableUpdateAction,
			public calendar::BaseCalendarUpdateAction
		{
		public:
			static const std::string PARAMETER_ROUTE_ID;
			static const std::string PARAMETER_TRANSPORT_MODE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_DIRECTION;
			static const std::string PARAMETER_DIRECTION_ID;
			static const std::string PARAMETER_WAYBACK;
			static const std::string PARAMETER_MAIN;
			static const std::string PARAMETER_PLANNED_LENGTH;
			static const std::string PARAMETER_LINE_ID;

		private:
			boost::shared_ptr<JourneyPattern> _route;

			boost::optional<boost::shared_ptr<vehicle::RollingStock> > _transportMode;
			boost::optional<boost::shared_ptr<Destination> > _directionObj;
			boost::optional<std::string> _name;
			boost::optional<std::string> _direction;
			boost::optional<bool> _wayback;
			boost::optional<bool> _main;
			boost::optional<graph::MetricOffset> _plannedLength;
			boost::optional<boost::shared_ptr<CommercialLine> > _line;


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_pattern_update
			//////////////////////////////////////////////////////////////////////////
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_pattern_update
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Journey_pattern_update
			//////////////////////////////////////////////////////////////////////////
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
			//@}
		};
}	}

#endif // SYNTHESE_LineUpdateAction_H__
