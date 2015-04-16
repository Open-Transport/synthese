
//////////////////////////////////////////////////////////////////////////
/// HikingTrailUpdateAction class header.
///	@file HikingTrailUpdateAction.h
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

#ifndef SYNTHESE_HikingTrailUpdateAction_H__
#define SYNTHESE_HikingTrailUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace hiking
	{
		class HikingTrail;

		//////////////////////////////////////////////////////////////////////////
		/// 58.15 Action : HikingTrailUpdateAction.
		/// @ingroup m58Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : HikingTrailUpdateAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the object to update</li>
		///		<li>actionParamna : name</li>
		///		<li>actionParamma : map image URL</li>
		///		<li>actionParamdu : duration image URL</li>
		///		<li>actionParampr : profile image URL</li>
		///		<li>actionParamur : link URL</li>
		///	</ul>
		class HikingTrailUpdateAction:
			public util::FactorableTemplate<server::Action, HikingTrailUpdateAction>
		{
		public:
			static const std::string PARAMETER_TRAIL_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_MAP;
			static const std::string PARAMETER_DURATION;
			static const std::string PARAMETER_PROFILE;
			static const std::string PARAMETER_URL;

		private:
			boost::shared_ptr<HikingTrail> _trail;
			std::string _name;
			std::string _map;
			std::string _duration;
			std::string _profile;
			std::string _url;

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

#endif // SYNTHESE_HikingTrailUpdateAction_H__
