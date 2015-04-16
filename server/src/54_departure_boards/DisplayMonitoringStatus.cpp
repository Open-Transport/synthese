////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatus class implementation.
///	@file DisplayMonitoringStatus.cpp
///	@author Hugues Romain
///	@date 2008-12-19 2:18
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayMonitoringStatus.h"
#include "DisplayScreen.h"

#include <sstream>
#include <assert.h>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<departure_boards::DisplayMonitoringStatus>::KEY("DisplayMonitoringStatus");
	}

	namespace departure_boards
	{


		DisplayMonitoringStatus::DisplayMonitoringStatus(
			util::RegistryKeyType id
		):	Registrable(id),
			_displayScreen(NULL),
			_cpu(NULL),
			_time(second_clock::local_time()),
			_generalStatus(DISPLAY_MONITORING_UNKNOWN),
			_memoryStatus(DISPLAY_MONITORING_UNKNOWN),
			_clockStatus(DISPLAY_MONITORING_UNKNOWN),
			_eepromStatus(DISPLAY_MONITORING_UNKNOWN),
			_tempSensorStatus(DISPLAY_MONITORING_UNKNOWN),
			_lightStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayStatus(DISPLAY_MONITORING_UNKNOWN),
			_soundStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureStatus(DISPLAY_MONITORING_UNKNOWN),
			_communicationStatus(DISPLAY_MONITORING_UNKNOWN),
			_localizationStatus(DISPLAY_MONITORING_UNKNOWN)
		{

		}



		DisplayMonitoringStatus::DisplayMonitoringStatus(
			const std::string& monitoringInterfaceReturn,
			const DisplayScreen* screen
		):	Registrable(0),
			_displayScreen(screen),
			_cpu(NULL),
			_time(second_clock::local_time()),
			_generalStatus(DISPLAY_MONITORING_UNKNOWN),
			_memoryStatus(DISPLAY_MONITORING_UNKNOWN),
			_clockStatus(DISPLAY_MONITORING_UNKNOWN),
			_eepromStatus(DISPLAY_MONITORING_UNKNOWN),
			_tempSensorStatus(DISPLAY_MONITORING_UNKNOWN),
			_lightStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayStatus(DISPLAY_MONITORING_UNKNOWN),
			_soundStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureStatus(DISPLAY_MONITORING_UNKNOWN),
			_communicationStatus(DISPLAY_MONITORING_UNKNOWN),
			_localizationStatus(DISPLAY_MONITORING_UNKNOWN)
		{
			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep1 ("|","",keep_empty_tokens);
			tokenizer tokens(monitoringInterfaceReturn, sep1);
			tokenizer::iterator it(tokens.begin());

			// General
			if (it == tokens.end()) return;
			setGeneralStatus(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR);

			// Memory
			if (++it == tokens.end()) return;
			setMemoryStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Clock
			if (++it == tokens.end()) return;
			setClockStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// EEPROM
			if (++it == tokens.end()) return;
			setEepromStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Temperature sensor
			if (++it == tokens.end()) return;
			setTempSensorStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Light
			if (++it == tokens.end()) return;
			setLightStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setLightDetail(*it);

			// Display
			if (++it == tokens.end()) return;
			setDisplayStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setDisplayDetail(*it);

			// Sound
			if (++it == tokens.end()) return;
			setSoundStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);
			if (++it == tokens.end()) return;
			setSoundDetail(*it);

			// Temperature
			if (++it == tokens.end()) return;
			if (lexical_cast<int>(*it) == UNKNOWN_VALUE)
			{
				setTemperatureStatus(DISPLAY_MONITORING_UNKNOWN);
				if (++it == tokens.end()) return;
			}
			else
			{
				if (lexical_cast<bool>(*it))
				{
					setTemperatureStatus(DISPLAY_MONITORING_OK);
					if (++it == tokens.end()) return;
				}
				else
				{
					if (++it == tokens.end()) return;
					setTemperatureStatus(
						(lexical_cast<int>(*it) == UNKNOWN_VALUE || !lexical_cast<bool>(*it)) ?
						DISPLAY_MONITORING_ERROR :
						DISPLAY_MONITORING_WARNING
					);
				}
			}
			if (++it == tokens.end()) return;
			setTemperatureValue((it->empty() || *it == "-1") ? optional<double>() : lexical_cast<double>(*it));

			// Communication
			if (++it == tokens.end()) return;
			setCommunicationStatus(
				(lexical_cast<int>(*it) == UNKNOWN_VALUE) ?
				DISPLAY_MONITORING_UNKNOWN :
				(lexical_cast<bool>(*it) ? DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR)
			);

			// Localization
			if (_displayScreen != NULL)
			{
				_localizationStatus = (_displayScreen->getLocation() != NULL) ?
					DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR;
			}
		}

		DisplayMonitoringStatus::DisplayMonitoringStatus(
			const DisplayScreenCPU* cpu
		):	Registrable(0),
			_displayScreen(NULL),
			_cpu(cpu),
			_time(second_clock::local_time()),
			_generalStatus(DISPLAY_MONITORING_UNKNOWN),
			_memoryStatus(DISPLAY_MONITORING_UNKNOWN),
			_clockStatus(DISPLAY_MONITORING_UNKNOWN),
			_eepromStatus(DISPLAY_MONITORING_UNKNOWN),
			_tempSensorStatus(DISPLAY_MONITORING_UNKNOWN),
			_lightStatus(DISPLAY_MONITORING_UNKNOWN),
			_displayStatus(DISPLAY_MONITORING_UNKNOWN),
			_soundStatus(DISPLAY_MONITORING_UNKNOWN),
			_temperatureStatus(DISPLAY_MONITORING_UNKNOWN),
			_communicationStatus(DISPLAY_MONITORING_UNKNOWN),
			_localizationStatus(DISPLAY_MONITORING_UNKNOWN)
		{
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



		void DisplayMonitoringStatus::setCommunicationStatus(
			DisplayMonitoringStatus::Status value
			) {
			_communicationStatus = value;
		}



		const ptime& DisplayMonitoringStatus::getTime(
		) const {
			return _time;
		}



		void DisplayMonitoringStatus::setTime(
		const ptime& value
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
				s << " / Temp: ";
				if(getTemperatureValue())
				{
					s << *getTemperatureValue() << "°";
				}
				else
				{
					s << "unknown";
				}
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



		std::string DisplayMonitoringStatus::GetStatusIcon(
			Status value
		) {
			switch(value)
			{
			case DISPLAY_MONITORING_OK:
				return "accept.png";

			case DISPLAY_MONITORING_ERROR:
				return "exclamation.png";

			case DISPLAY_MONITORING_UNKNOWN:
				return "help.png";

			case DISPLAY_MONITORING_WARNING:
				return "error.png";
			}
			assert(false);
			return string();
		}

		const DisplayScreenCPU* DisplayMonitoringStatus::getCPU() const
		{
			return _cpu;
		}

		void DisplayMonitoringStatus::setCPU( const DisplayScreenCPU* value )
		{
			_cpu = value;
		}
	}
}
