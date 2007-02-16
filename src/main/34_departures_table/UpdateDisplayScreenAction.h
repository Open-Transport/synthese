
/** UpdateDisplayScreenAction class header.
	@file UpdateDisplayScreenAction.h

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

#ifndef SYNTHESE_UpdateDisplayScreenAction_H__
#define SYNTHESE_UpdateDisplayScreenAction_H__

#include <map>

#include "30_server/Action.h"

#include "34_departures_table/Types.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
	}

	namespace departurestable
	{
		class BroadcastPoint;
		class DisplayScreen;
		class DisplayType;
		
		/** UpdateDisplayScreenAction action class.
			@ingroup m34
		*/
		class UpdateDisplayScreenAction : public server::Action
		{
		public:
			static const std::string PARAMETER_LOCALIZATION_ID;
			static const std::string PARAMETER_LOCALIZATION_COMMENT;
			static const std::string PARAMETER_WIRING_CODE;
			static const std::string PARAMETER_BLINKING_DELAY;
			static const std::string PARAMETER_CLEANING_DELAY;
			static const std::string PARAMETER_DISPLAY_PLATFORM;
			static const std::string PARAMETER_DISPLAY_SERVICE_NUMBER;
			static const std::string PARAMETER_DISPLAY_DEPARTURE_ARRIVAL;
			static const std::string PARAMETER_DISPLAY_END_FILTER;
			static const std::string PARAMETER_DISPLAY_MAX_DELAY;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_TITLE;

		private:
			DisplayScreen*								_screen;
			BroadcastPoint*								_localization;
			std::string									_localizationComment;
			std::string									_title;
			int											_wiringCode;
			int											_blinkingDelay;
			int											_cleaningDelay;
			bool										_displayPlatform;
			bool										_displayServiceNumber;
			DeparturesTableDirection					_direction;
			EndFilter									_endFilter;
			int											_maxDelay;
			DisplayType*								_type;
			std::map<const env::PhysicalStop*, bool>	_physicalStopServe;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::Request::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void setFromParametersMap(server::Request::ParametersMap& map);

		public:
			UpdateDisplayScreenAction();
			~UpdateDisplayScreenAction();

			/** Action to run, defined by each subclass.
			*/
			void run();
		};
	}
}

#endif // SYNTHESE_UpdateDisplayScreenAction_H__
