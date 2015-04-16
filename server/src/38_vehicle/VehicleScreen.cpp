
/** VehicleScreen class implementation.
    @file VehicleScreen.cpp

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

#include "VehicleScreen.hpp"

#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<>
        const std::string Registry<VehicleScreen>::KEY("VehicleScreen");
	}

	namespace vehicle
	{
		const string VehicleScreen::ATTR_NAME = "screen_name";
		const string VehicleScreen::ATTR_CONNECTED = "connected";
		const string VehicleScreen::ATTR_BACKLIGHT1_OK = "backlight1_ok";
		const string VehicleScreen::ATTR_BACKLIGHT2_OK = "backlight2_ok";
		const string VehicleScreen::ATTR_BACKLIGHT_AUTOMATIC_MODE = "backlight_automatic_mode";
        const string VehicleScreen::ATTR_BACKLIGHT_VALUE = "backlight_value";
        const string VehicleScreen::ATTR_BACKLIGHT_MIN = "backlight_min";
        const string VehicleScreen::ATTR_BACKLIGHT_MAX = "backlight_max";
        const string VehicleScreen::ATTR_BACKLIGHT_SPEED = "backlight_speed";

        VehicleScreen::VehicleScreen(
			RegistryKeyType id
		):	Registrable(id),
			_screen(NULL),
            _connected(false),
			_backlight1_OK(false),
			_backlight2_OK(false),
			_backlightAutomaticMode(false),
            _backlightValue(false),
            _backlightMin(false),
            _backlightMax(false),
            _backlightSpeed(false)
        {}



		void VehicleScreen::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			pm.insert(ATTR_NAME, _name);
			pm.insert(ATTR_CONNECTED, _connected);
			pm.insert(ATTR_BACKLIGHT1_OK, _backlight1_OK);
			pm.insert(ATTR_BACKLIGHT2_OK, _backlight2_OK);
			pm.insert(ATTR_BACKLIGHT_AUTOMATIC_MODE, _backlightAutomaticMode);
			pm.insert(ATTR_BACKLIGHT_VALUE, _backlightValue);
			pm.insert(ATTR_BACKLIGHT_MIN, _backlightMin);
			pm.insert(ATTR_BACKLIGHT_MAX, _backlightMax);
			pm.insert(ATTR_BACKLIGHT_SPEED, _backlightSpeed);
		}
}	}
