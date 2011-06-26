
/** DisplayScreenAppearanceUpdateAction class header.
	@file DisplayScreenAppearanceUpdateAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_DisplayScreenAppearanceUpdateAction_H__
#define SYNTHESE_DisplayScreenAppearanceUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;

		/** Display screen appearance update action class.
			@ingroup m54Actions refActions
		*/
		class DisplayScreenAppearanceUpdateAction
			: public util::FactorableTemplate<server::Action, DisplayScreenAppearanceUpdateAction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_DISPLAY_PLATFORM;
			static const std::string PARAMETER_DISPLAY_SERVICE_NUMBER;
			static const std::string PARAMETER_DISPLAY_TEAM;
			static const std::string PARAMETER_DISPLAY_CLOCK;
			static const std::string PARAMETER_BLINKING_DELAY;

		private:
			boost::shared_ptr<DisplayScreen>	_screen;
			int									_blinkingDelay;
			bool								_displayPlatform;
			bool								_displayServiceNumber;
			bool								_displayTeam;
			bool								_displayClock;
			std::string							_title;


		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);



			////////////////////////////////////////////////////////////////////
			///	Authorization test.
			///	@return bool true if the user can use the action.
			///	@author Hugues Romain
			///	@date 2008
			virtual bool isAuthorized(const server::Session* session) const;


		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			DisplayScreenAppearanceUpdateAction();

			void setScreenId(util::RegistryKeyType id);
		};
	}
}

#endif // SYNTHESE_DisplayScreenAppearanceUpdateAction_H__
