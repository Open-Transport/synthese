
/** DisplayScreenAddDisplayedPlaceAction class header.
	@file DisplayScreenAddDisplayedPlaceAction.h

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

#ifndef SYNTHESE_DisplayScreenAddDisplayedPlaceAction_H__
#define SYNTHESE_DisplayScreenAddDisplayedPlaceAction_H__

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
		/// 54.15 Action : adds a displayed place to a display screen.
		/// @ingroup m54Actions refActions
		//////////////////////////////////////////////////////////////////////////
		/// The place to add can be specified in two ways :
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
		///
		class DisplayScreenAddDisplayedPlaceAction:
			public util::FactorableTemplate<server::Action, DisplayScreenAddDisplayedPlaceAction>
		{
		public:
			static const std::string PARAMETER_SCREEN_ID;
			static const std::string PARAMETER_PLACE;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;

		private:
			boost::shared_ptr<DisplayScreen>			_screen;
			boost::shared_ptr<const pt::StopArea> _place;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setScreen(boost::shared_ptr<const DisplayScreen> value);
		};
	}
}

#endif // SYNTHESE_DisplayScreenAddDisplayedPlace_H__
