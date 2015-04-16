
//////////////////////////////////////////////////////////////////////////
/// HikingTrailStopAddAction class header.
///	@file HikingTrailStopAddAction.hpp
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

#ifndef SYNTHESE_HikingTrailStopAddAction_H__
#define SYNTHESE_HikingTrailStopAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace hiking
	{
		class HikingTrail;

		//////////////////////////////////////////////////////////////////////////
		/// 58.15 Action : HikingTrailStopAddAction.
		/// @ingroup m58Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : HikingTrailStopAddAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the trail to add the stop to</li>
		///		<li>actionParamci : city of the stop</li>
		///		<li>actionParamna : name of the stop</li>
		///		<li>actionParamrk : rank</li>
		///	</ul>
		class HikingTrailStopAddAction:
			public util::FactorableTemplate<server::Action, HikingTrailStopAddAction>
		{
		public:
			static const std::string PARAMETER_TRAIL_ID;
			static const std::string PARAMETER_CITY;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_RANK;

		private:
			boost::shared_ptr<HikingTrail> _trail;
			boost::shared_ptr<pt::StopArea> _stop;
			boost::optional<std::size_t> _rank;

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
				void setTrail(boost::shared_ptr<HikingTrail> value) { _trail = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_HikingTrailStopAddAction_H__
