////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatus class implementation.
///	@file DisplayMonitoringStatus.cpp
///	@author Hugues Romain
///	@date 2008-12-19 2:18
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayMonitoringStatus.h"
#include "DisplayScreen.h"
#include "Conversion.h"

#include <sstream>
#include <assert.h>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace time;
	using namespace util;

	namespace util
	{
		template<> const string Registry<departurestable::DisplayMonitoringStatus>::KEY("DisplayMonitoringStatus");
	}

	namespace departurestable
	{


		DisplayMonitoringStatus::DisplayMonitoringStatus(
			util::RegistryKeyType id /*= UNKNOWN_VALUE */
		):	Registrable(id),
			_generalStatus(DISPLAY_MONITORING_UNKNOWN),
			_memoryStatus(DISPLAY_MONITORING_UNKNOWN),
			_clockStatus(DISPLAY_MONITORING_UNKNOWN),
			_eepromStatus(DISPLAY_MONITORING_UNKNOWN),
			_tempSensorStatus(DISPLAY_MONITORING_UNKNOWN),
			_lightStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayStatus(DISPLAY_MONITORING_UNKNOWN),
			_soundStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureValue(static_cast<double>(UNKNOWN_VALUE)),
			_communicationStatus(DISPLAY_MONITORING_UNKNOWN),
			_localizationStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayScreen(NULL),
			_time(TIME_CURRENT)
		{

		}



		DisplayMonitoringStatus::DisplayMonitoringStatus(
			const std::string& monitoringInterfaceReturn,
			const DisplayScreen* screen
		):	Registrable(UNKNOWN_VALUE),
			_generalStatus(DISPLAY_MONITORING_UNKNOWN),
			_memoryStatus(DISPLAY_MONITORING_UNKNOWN),
			_clockStatus(DISPLAY_MONITORING_UNKNOWN),
			_eepromStatus(DISPLAY_MONITORING_UNKNOWN),
			_tempSensorStatus(DISPLAY_MONITORING_UNKNOWN),
			_lightStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayStatus(DISPLAY_MONITORING_UNKNOWN),
			_soundStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureValue(static_cast<double>(UNKNOWN_VALUE)),
			_communicationStatus(DISPLAY_MONITORING_UNKNOWN),
			_localizationStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayScreen(screen),
			_time(TIME_CURRENT)
		{
			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep1 ("|","",keep_empty_tokens);
			tokenizer tokens(monitoringInterfaceReturn, sep1);
			tokenizer::iterator it(tokens.begin());

			// General
			if (it == tokens.end()) return;
			setGeneralStatus(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR);

			// Memory
			if (++it == tokens.end()) return;
			setMemoryStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Clock
			if (++it == tokens.end()) return;
			setClockStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// EEPROM
			if (++it == tokens.end()) return;
			setEepromStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Temperature sensor
			if (++it == tokens.end()) return;
			setTempSensorStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Light
			if (++it == tokens.end()) return;
			setLightStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setLightDetail(*it);

			// Display
			if (++it == tokens.end()) return;
			setDisplayStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setDisplayDetail(*it);

			// Sound
			if (++it == tokens.end()) return;
			setSoundStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setSoundDetail(*it);

			// Temperature
			if (++it == tokens.end()) return;
			if (Conversion::ToInt(*it) == UNKNOWN_VALUE)
			{
				setTemperatureStatus(DISPLAY_MONITORING_UNKNOWN);
				if (++it == tokens.end()) return;
			}
			else
			{
				if (Conversion::ToBool(*it))
				{
					setTemperatureStatus(DISPLAY_MONITORING_OK);
					if (++it == tokens.end()) return;
				}
				else
				{
					if (++it == tokens.end()) return;
					setTemperatureStatus(
						(Conversion::ToInt(*it) == UNKNOWN_VALUE || !Conversion::ToBool(*it)) ?
						DISPLAY_MONITORING_ERROR :
						DISPLAY_MONITORING_WARNING
					);
				}
			}
			if (++it == tokens.end()) return;
			setTemperatureValue(Conversion::ToDouble(*it));

			// Communication
			if (++it == tokens.end()) return;
			setCommunicationStatus(
				(Conversion::ToInt(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(Conversion::ToBool(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Localization
			if (_displayScreen != NULL)
			{
				_localizationStatus = (_displayScreen->getLocalization() != NULL) ?
					DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR;
			}
		}


		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getGeneralStatus(
		) const {
			return _generalStatus;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getMemoryStatus(

			) const {
			return _memoryStatus;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getClockStatus(

			) const {
			return _clockStatus;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getEepromStatus(

			) const {
			return _eepromStatus;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getTempSensorStatus(

			) const {
			return _temperatureStatus;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getDisplayStatus(

			) const {
			return _displayStatus;
		}



		const std::string& DisplayMonitoringStatus::getDisplayDetail(

			) const {
			return _displayDetail;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getSoundStatus(

			) const {
			return _soundStatus;
		}



		const std::string& DisplayMonitoringStatus::getSoundDetail(

			) const {
			return _displayDetail;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getTemperatureStatus(

			) const {
			return _temperatureStatus;
		}



		double DisplayMonitoringStatus::getTemperatureValue(

			) const {
			return _temperatureValue;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getCommunicationStatus(

			) const {
			return _communicationStatus;
		}



		void DisplayMonitoringStatus::setGeneralStatus(
			DisplayMonitoringStatus::Status value
			) {
			_generalStatus = value;
		}



		void DisplayMonitoringStatus::setMemoryStatus(
			DisplayMonitoringStatus::Status value
			) {
			_memoryStatus = value;
		}



		void DisplayMonitoringStatus::setClockStatus(
			DisplayMonitoringStatus::Status value
			) {
			_clockStatus = value;
		}



		void DisplayMonitoringStatus::setEepromStatus(
			DisplayMonitoringStatus::Status value
			) {
			_eepromStatus = value;
		}



		void DisplayMonitoringStatus::setTempSensorStatus(
			DisplayMonitoringStatus::Status value
			) {
			_tempSensorStatus = value;
		}



		void DisplayMonitoringStatus::setDisplayStatus(
			DisplayMonitoringStatus::Status value
			) {
			_displayStatus = value;
		}



		void DisplayMonitoringStatus::setDisplayDetail(
			const std::string& value
			) {
			_displayDetail = value;
		}



		void DisplayMonitoringStatus::setSoundStatus(
			DisplayMonitoringStatus::Status value
			) {
			_soundStatus = value;
		}



		void DisplayMonitoringStatus::setSoundDetail(
			const std::string& value
			) {
			_soundDetail = value;
		}



		void DisplayMonitoringStatus::setTemperatureStatus(
			DisplayMonitoringStatus::Status value
			) {
			_temperatureStatus = value;
		}



		void DisplayMonitoringStatus::setTemperatureValue(
			double value
			) {
			_temperatureValue = value;
		}



		void DisplayMonitoringStatus::setCommunicationStatus(
			DisplayMonitoringStatus::Status value
			) {
			_communicationStatus = value;
		}



		const time::DateTime DisplayMonitoringStatus::getTime(
		) const {
			return _time;
		}



		void DisplayMonitoringStatus::setTime(
		const time::DateTime& value
			) {
			_time = value;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getLightStatus(

			) const {
			return _lightStatus;
		}



		const std::string& DisplayMonitoringStatus::getLightDetail(

			) const {
			return _lightDetail;
		}



		void DisplayMonitoringStatus::setLightStatus(
			DisplayMonitoringStatus::Status value
		) {
			_lightStatus = value;
		}



		void DisplayMonitoringStatus::setLightDetail(
			const std::string& value
			) {
			_lightDetail = value;
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getGlobalStatus(
		) const {
			// Attempt to return OK
			if(	_generalStatus == DISPLAY_MONITORING_OK	&&
				(_memoryStatus == DISPLAY_MONITORING_OK || _memoryStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_clockStatus == DISPLAY_MONITORING_OK || _clockStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_eepromStatus == DISPLAY_MONITORING_OK || _eepromStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_tempSensorStatus == DISPLAY_MONITORING_OK || _tempSensorStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_lightStatus == DISPLAY_MONITORING_OK || _lightStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_displayStatus == DISPLAY_MONITORING_OK || _displayStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_soundStatus == DISPLAY_MONITORING_OK || _soundStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_temperatureStatus == DISPLAY_MONITORING_OK || _temperatureStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_communicationStatus == DISPLAY_MONITORING_OK || _communicationStatus == DISPLAY_MONITORING_UNKNOWN) &&
				(_localizationStatus == DISPLAY_MONITORING_OK || _localizationStatus == DISPLAY_MONITORING_UNKNOWN)
			){
				return DISPLAY_MONITORING_OK;
			}

			// Attempt to return WARNING
			if(	(_generalStatus == DISPLAY_MONITORING_OK || _generalStatus == DISPLAY_MONITORING_WARNING) 	&&
				(_memoryStatus == DISPLAY_MONITORING_OK || _memoryStatus == DISPLAY_MONITORING_UNKNOWN || _memoryStatus == DISPLAY_MONITORING_WARNING) &&
				(_eepromStatus == DISPLAY_MONITORING_OK || _eepromStatus == DISPLAY_MONITORING_UNKNOWN || _eepromStatus == DISPLAY_MONITORING_WARNING) &&
				(_displayStatus == DISPLAY_MONITORING_OK || _displayStatus == DISPLAY_MONITORING_UNKNOWN || _displayStatus == DISPLAY_MONITORING_WARNING) &&
				(_tempSensorStatus == DISPLAY_MONITORING_ERROR || _temperatureStatus == DISPLAY_MONITORING_OK || _temperatureStatus == DISPLAY_MONITORING_UNKNOWN || _temperatureStatus == DISPLAY_MONITORING_WARNING) &&
				(_communicationStatus == DISPLAY_MONITORING_OK || _communicationStatus == DISPLAY_MONITORING_UNKNOWN || _communicationStatus == DISPLAY_MONITORING_WARNING) &&
				(_localizationStatus == DISPLAY_MONITORING_OK || _localizationStatus == DISPLAY_MONITORING_UNKNOWN || _localizationStatus == DISPLAY_MONITORING_WARNING)
			){
				return DISPLAY_MONITORING_WARNING;
			}

			// Else return ERROR
			return DISPLAY_MONITORING_ERROR;
		}



		std::string DisplayMonitoringStatus::getDetail(
		) const {
			stringstream s;
			s << "Status: " << GetStatusString(getGlobalStatus());
			if (getTempSensorStatus() != DISPLAY_MONITORING_ERROR)
			{
				s << " / Temp: " << getTemperatureValue() << "°";
			}
			if (getGeneralStatus() != DISPLAY_MONITORING_OK && getGeneralStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / General: " << GetStatusString(getGeneralStatus());
			}
			if (getMemoryStatus() != DISPLAY_MONITORING_OK && getMemoryStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / Memory: " << GetStatusString(getMemoryStatus());
			}
			if (getClockStatus() != DISPLAY_MONITORING_OK && getClockStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / Clock: " << GetStatusString(getClockStatus());
			}
			if (getEepromStatus() != DISPLAY_MONITORING_OK && getEepromStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / EEPROM: " << GetStatusString(getEepromStatus());
			}
			if (getTempSensorStatus() != DISPLAY_MONITORING_OK && getTempSensorStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / Temperature sensor: " << GetStatusString(getTempSensorStatus());
			}
			if (getLightStatus() != DISPLAY_MONITORING_OK && getLightStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s	<< " / Light: " << GetStatusString(getLightStatus())
					<< " " << getLightDetail();
			}
			if (getDisplayStatus() != DISPLAY_MONITORING_OK && getDisplayStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s	<< " / Display: " << GetStatusString(getDisplayStatus())
					<< " " << getDisplayDetail();
			}
			if (getSoundStatus() != DISPLAY_MONITORING_OK && getSoundStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s	<< " / Sound: " << GetStatusString(getSoundStatus())
					<< " " << getSoundDetail();
			}
			if (getTempSensorStatus() != DISPLAY_MONITORING_OK && getTempSensorStatus() != DISPLAY_MONITORING_UNKNOWN && getTempSensorStatus() != DISPLAY_MONITORING_ERROR)
			{
				s << " / Temperature alert: " << GetStatusString(getTemperatureStatus());
			}
			if (getCommunicationStatus() != DISPLAY_MONITORING_OK && getCommunicationStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / Communication: " << GetStatusString(getCommunicationStatus());
			}
			if (getLocalizationStatus() != DISPLAY_MONITORING_OK && getLocalizationStatus() != DISPLAY_MONITORING_UNKNOWN)
			{
				s << " / Data localization: " << GetStatusString(getLocalizationStatus());
			}
			return s.str();
		}



		std::string DisplayMonitoringStatus::GetStatusString(
			DisplayMonitoringStatus::Status value
		){
			switch(value)
			{
			case DISPLAY_MONITORING_OK:
				return "OK";
			
			case DISPLAY_MONITORING_ERROR:
				return "Error";
			
			case DISPLAY_MONITORING_UNKNOWN:
				return "Unknown";
			
			case DISPLAY_MONITORING_WARNING:
				return "Warning";
			}
			assert(false);
			return string();
		}



		DisplayMonitoringStatus::Status DisplayMonitoringStatus::getLocalizationStatus(
		) const {
			return _localizationStatus;
		}



		void DisplayMonitoringStatus::setLocalizationStatus(
			DisplayMonitoringStatus::Status value
		){
			_localizationStatus = value;
		}



		void DisplayMonitoringStatus::setScreen(
			const DisplayScreen* value
		) {
			_displayScreen = value;
		}



		const DisplayScreen* DisplayMonitoringStatus::getScreen(
		) const {
			return _displayScreen;
		}



		DisplayMonitoringStatus::~DisplayMonitoringStatus(

			) {

		}
	}
}
