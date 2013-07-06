
/** VehicleScreen class header.
	@file VehicleScreen.hpp

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

#ifndef SYNTHESE_pt_operation_VehicleScreen_hpp__
#define SYNTHESE_pt_operation_VehicleScreen_hpp__

#include "Registrable.h"
#include "Registry.h"
#include "WithGeometry.hpp"


namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		class Vehicle;

		/** VehicleScreen class.
			@ingroup m38
		*/
		class VehicleScreen:
			public util::Registrable
		{
		private:
			static const std::string ATTR_NAME;
			static const std::string ATTR_CONNECTED;
			static const std::string ATTR_BACKLIGHT1_OK;
			static const std::string ATTR_BACKLIGHT2_OK;
			static const std::string ATTR_BACKLIGHT_AUTOMATIC_MODE;
			static const std::string ATTR_BACKLIGHT_VALUE;
			static const std::string ATTR_BACKLIGHT_MIN;
			static const std::string ATTR_BACKLIGHT_MAX;
			static const std::string ATTR_BACKLIGHT_SPEED;

		public:
			typedef util::Registry<VehicleScreen> Registry;



		private:
			std::string _name;
			bool _connected;
			bool _backlight1_OK;
			bool _backlight2_OK;
			bool _backlightAutomaticMode;
			int _backlightValue;
			int _backlightMin;
			int _backlightMax;
			int _backlightSpeed;

		public:
			VehicleScreen(
				util::RegistryKeyType id = 0
			);

			void setName(const std::string& value){ _name = value; }
			std::string getName(){ return _name; }

			void setConnected(bool value){ _connected = value; }
			bool getConnected(){ return _connected; }

			// Our screen is controlled by 2 backlights
			void setBacklight1_OK(bool value){ _backlight1_OK = value; }
			bool getBacklight1_OK(){ return _backlight1_OK; }

			void setBacklight2_OK(bool value){ _backlight2_OK = value; }
			bool getBacklight2_OK(){ return _backlight2_OK; }

			void setBacklightAutomaticMode(bool value){ _backlightAutomaticMode = value; }
			bool getBacklightAutomaticMode(){ return _backlightAutomaticMode; }

			void setBacklightValue(int value){ _backlightValue = value; }
			int getBacklightValue(){ return _backlightValue; }
			
			void setBacklightMin(int value){ _backlightMin = value; }
			int getBacklightMin(){ return _backlightMin; }

			void setBacklightMax(int value){ _backlightMax = value; }
			int getBacklightMax(){ return _backlightMax; }

			void setBacklightSpeed(int value){ _backlightSpeed = value; }
			int getBacklightSpeed(){ return _backlightSpeed; }

			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;
		};
}	}

#endif // SYNTHESE_pt_operation_VehicleScreen_hpp__
