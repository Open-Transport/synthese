
//////////////////////////////////////////////////////////////////////////
/// DisplayScreenUpdateDisplayedStopAreaAction class header.
///	@file DisplayScreenUpdateDisplayedStopAreaAction.hpp
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

#ifndef SYNTHESE_DisplayScreenUpdateDisplayedStopAreaAction_H__
#define SYNTHESE_DisplayScreenUpdateDisplayedStopAreaAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		//////////////////////////////////////////////////////////////////////////
		/// 54.15 Action : DisplayScreenUpdateDisplayedStopAreaAction.
		/// @ingroup m54Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : DisplayScreenUpdateDisplayedStopAreaAction
		///
		/// The place to display can be specified in two ways :
		///	<ul>
		///		<li>directly by a place ID<br />
		///		Parameters :</li>
		///		<ul>
		///			<li>actionParams : id of the screen to add the place to</li>
		///			<li>actionParampl : place ID</li>
		///		</ul>
		///		<li>by city and place names : in this case the lexical matcher is used to determinate
		///			the nearest choice<br />
		///		Parameters :</li>
		///		<ul>
		///			<li>actionParams : id of the screen to add the place to</li>
		///			<li>actionParamcn : Name of the city of the place</li>
		///			<li>actionParampn : Name of the place in the city</li>
		///		</ul>
		///	</ul>
		class DisplayScreenUpdateDisplayedStopAreaAction:
			public util::FactorableTemplate<server::Action, DisplayScreenUpdateDisplayedStopAreaAction>
		{
		public:
			static const std::string PARAMETER_SCREEN_ID;
			static const std::string PARAMETER_PLACE;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;

		private:
			boost::shared_ptr<DisplayScreen>		_screen;
			boost::shared_ptr<const pt::StopArea>	_place;


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
				void setScreen(boost::shared_ptr<DisplayScreen> value){ _screen = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_DisplayScreenUpdateDisplayedStopAreaAction_H__
