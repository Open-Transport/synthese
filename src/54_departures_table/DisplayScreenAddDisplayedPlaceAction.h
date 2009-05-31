
/** DisplayScreenAddDisplayedPlaceAction class header.
	@file DisplayScreenAddDisplayedPlaceAction.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		class DisplayScreen;

		/** Adds a displayed place to a display screen.
			@ingroup m54Actions refActions

			The place to add can be specified in two ways :
				-  directly by a place ID
				-  by city and place names : in this case the lexical matcher is used to determinate
					the nearest choice
		*/
		class DisplayScreenAddDisplayedPlaceAction:
			public util::FactorableTemplate<server::Action, DisplayScreenAddDisplayedPlaceAction>
		{
		public:
			static const std::string PARAMETER_PLACE;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_PLACE_NAME;

		private:
			boost::shared_ptr<DisplayScreen>			_screen;
			boost::shared_ptr<const env::PublicTransportStopZoneConnectionPlace> _placeSptr;
			const env::PublicTransportStopZoneConnectionPlace*	_place;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_DisplayScreenAddDisplayedPlace_H__
