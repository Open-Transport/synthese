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

#include "DisplayMaintenanceRight.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenTableSync.h"
#include "Profile.h"
#include "User.h"

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
	using namespace departure_boards;
	using namespace util;

	CLASS_DEFINITION(DisplayMonitoringStatus, "t057_display_monitoring_status", 57)
	FIELD_DEFINITION_OF_OBJECT(DisplayMonitoringStatus, "display_monitoring_status_id", "display_monitoring_status_ids")

	FIELD_DEFINITION_OF_TYPE(StatusScreen, "screen_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(StatusTime, "time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(GeneralStatus, "general_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MemoryStatus, "memory_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ClockStatus, "clock_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(EepromStatus, "eeprom_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TempSensorStatus, "temp_sensor_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LightStatus, "light_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LightDetail, "light_detail", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DisplayStatus, "display_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayDetail, "display_detail", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SoundStatus, "sound_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(SoundDetail, "sound_detail", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TemperatureStatus, "temperature_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TemperatureValue, "temperature_value", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(CommunicationStatus, "communication_status", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(LocalizationStatus, "localization_status", SQL_INTEGER)

	namespace departure_boards
	{


		DisplayMonitoringStatus::DisplayMonitoringStatus(
			util::RegistryKeyType id
		):	Registrable(id),
			Object<DisplayMonitoringStatus, DisplayMonitoringStatusSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(StatusScreen),
					FIELD_VALUE_CONSTRUCTOR(StatusTime, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(GeneralStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(MemoryStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(ClockStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(EepromStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(TempSensorStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LightStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(LightDetail),
					FIELD_VALUE_CONSTRUCTOR(DisplayStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayDetail),
					FIELD_VALUE_CONSTRUCTOR(SoundStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(SoundDetail),
					FIELD_VALUE_CONSTRUCTOR(TemperatureStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(TemperatureValue),
					FIELD_VALUE_CONSTRUCTOR(CommunicationStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LocalizationStatus, DISPLAY_MONITORING_UNKNOWN)
			)	),
			_displayScreen(NULL),
			_cpu(NULL)
		{

		}



		DisplayMonitoringStatus::DisplayMonitoringStatus(
			const std::string& monitoringInterfaceReturn,
			const DisplayScreen* screen
		):	Registrable(0),
			Object<DisplayMonitoringStatus, DisplayMonitoringStatusSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, 0),
					FIELD_DEFAULT_CONSTRUCTOR(StatusScreen),
					FIELD_VALUE_CONSTRUCTOR(StatusTime, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(GeneralStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(MemoryStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(ClockStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(EepromStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(TempSensorStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LightStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(LightDetail),
					FIELD_VALUE_CONSTRUCTOR(DisplayStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayDetail),
					FIELD_VALUE_CONSTRUCTOR(SoundStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(SoundDetail),
					FIELD_VALUE_CONSTRUCTOR(TemperatureStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(TemperatureValue),
					FIELD_VALUE_CONSTRUCTOR(CommunicationStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LocalizationStatus, DISPLAY_MONITORING_UNKNOWN)
			)	),
			_displayScreen(screen),
			_cpu(NULL)
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
				setLocalizationStatus(
					(_displayScreen->getLocation() != NULL) ?
					DISPLAY_MONITORING_OK : DISPLAY_MONITORING_ERROR
				);
			}
		}

		DisplayMonitoringStatus::DisplayMonitoringStatus(
			const DisplayScreenCPU* cpu
		):	Registrable(0),
			Object<DisplayMonitoringStatus, DisplayMonitoringStatusSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, 0),
					FIELD_DEFAULT_CONSTRUCTOR(StatusScreen),
					FIELD_VALUE_CONSTRUCTOR(StatusTime, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(GeneralStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(MemoryStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(ClockStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(EepromStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(TempSensorStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LightStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(LightDetail),
					FIELD_VALUE_CONSTRUCTOR(DisplayStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayDetail),
					FIELD_VALUE_CONSTRUCTOR(SoundStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(SoundDetail),
					FIELD_VALUE_CONSTRUCTOR(TemperatureStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_DEFAULT_CONSTRUCTOR(TemperatureValue),
					FIELD_VALUE_CONSTRUCTOR(CommunicationStatus, DISPLAY_MONITORING_UNKNOWN),
					FIELD_VALUE_CONSTRUCTOR(LocalizationStatus, DISPLAY_MONITORING_UNKNOWN)
			)	),
			_displayScreen(NULL),
			_cpu(cpu)
		{
		}


		Status DisplayMonitoringStatus::getGeneralStatus(
		) const {
			return get<GeneralStatus>();
		}



		Status DisplayMonitoringStatus::getMemoryStatus(
			) const {
			return get<MemoryStatus>();
		}



		Status DisplayMonitoringStatus::getClockStatus(
			) const {
			return get<ClockStatus>();
		}



		Status DisplayMonitoringStatus::getEepromStatus(
			) const {
			return get<EepromStatus>();
		}



		 Status DisplayMonitoringStatus::getTempSensorStatus(
			) const {
			return get<TemperatureStatus>();
		}



		Status DisplayMonitoringStatus::getDisplayStatus(
			) const {
			return get<DisplayStatus>();
		}



		const std::string& DisplayMonitoringStatus::getDisplayDetail(
			) const {
			return get<DisplayDetail>();
		}



		Status DisplayMonitoringStatus::getSoundStatus(
			) const {
			return get<SoundStatus>();
		}



		const std::string& DisplayMonitoringStatus::getSoundDetail(
			) const {
			return get<SoundDetail>();
		}



		Status DisplayMonitoringStatus::getTemperatureStatus(
			) const {
			return get<TemperatureStatus>();
		}



		Status DisplayMonitoringStatus::getCommunicationStatus(
			) const {
			return get<CommunicationStatus>();
		}



		void DisplayMonitoringStatus::setGeneralStatus(
			Status value
			) {
			set<GeneralStatus>(value);
		}



		void DisplayMonitoringStatus::setMemoryStatus(
			Status value
			) {
			set<MemoryStatus>(value);
		}



		void DisplayMonitoringStatus::setClockStatus(
			Status value
			) {
			set<ClockStatus>(value);
		}



		void DisplayMonitoringStatus::setEepromStatus(
			Status value
			) {
			set<EepromStatus>(value);
		}



		void DisplayMonitoringStatus::setTempSensorStatus(
			Status value
			) {
			set<TempSensorStatus>(value);
		}



		void DisplayMonitoringStatus::setDisplayStatus(
			Status value
			) {
			set<DisplayStatus>(value);
		}



		void DisplayMonitoringStatus::setDisplayDetail(
			const std::string& value
			) {
			set<DisplayDetail>(value);
		}



		void DisplayMonitoringStatus::setSoundStatus(
			Status value
			) {
			set<SoundStatus>(value);
		}



		void DisplayMonitoringStatus::setSoundDetail(
			const std::string& value
			) {
			set<SoundDetail>(value);
		}



		void DisplayMonitoringStatus::setTemperatureStatus(
			Status value
			) {
			set<TemperatureStatus>(value);
		}



		void DisplayMonitoringStatus::setCommunicationStatus(
			Status value
			) {
			set<CommunicationStatus>(value);
		}



		const ptime& DisplayMonitoringStatus::getTime(
		) const {
			return get<StatusTime>();
		}



		void DisplayMonitoringStatus::setTime(
		const ptime& value
		) {
			set<StatusTime>(value);
		}



		Status DisplayMonitoringStatus::getLightStatus(
			) const {
			return get<LightStatus>();
		}



		const std::string& DisplayMonitoringStatus::getLightDetail(
			) const {
			return get<LightDetail>();
		}



		void DisplayMonitoringStatus::setLightStatus(
			Status value
		) {
			set<LightStatus>(value);
		}



		void DisplayMonitoringStatus::setLightDetail(
			const std::string& value
			) {
			set<LightDetail>(value);
		}



		Status DisplayMonitoringStatus::getGlobalStatus(
		) const {
			// Attempt to return OK
			if(	getGeneralStatus() == DISPLAY_MONITORING_OK	&&
				(getMemoryStatus() == DISPLAY_MONITORING_OK || getMemoryStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getClockStatus() == DISPLAY_MONITORING_OK || getClockStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getEepromStatus() == DISPLAY_MONITORING_OK || getEepromStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getTempSensorStatus() == DISPLAY_MONITORING_OK || getTempSensorStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getLightStatus() == DISPLAY_MONITORING_OK || getLightStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getDisplayStatus() == DISPLAY_MONITORING_OK || getDisplayStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getSoundStatus() == DISPLAY_MONITORING_OK || getSoundStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getTemperatureStatus() == DISPLAY_MONITORING_OK || getTemperatureStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getCommunicationStatus() == DISPLAY_MONITORING_OK || getCommunicationStatus() == DISPLAY_MONITORING_UNKNOWN) &&
				(getLocalizationStatus() == DISPLAY_MONITORING_OK || getLocalizationStatus() == DISPLAY_MONITORING_UNKNOWN)
			){
				return DISPLAY_MONITORING_OK;
			}

			// Attempt to return WARNING
			if(	(getGeneralStatus() == DISPLAY_MONITORING_OK || getGeneralStatus() == DISPLAY_MONITORING_WARNING) 	&&
				(getMemoryStatus() == DISPLAY_MONITORING_OK || getMemoryStatus() == DISPLAY_MONITORING_UNKNOWN || getMemoryStatus() == DISPLAY_MONITORING_WARNING) &&
				(getEepromStatus() == DISPLAY_MONITORING_OK || getEepromStatus() == DISPLAY_MONITORING_UNKNOWN || getEepromStatus() == DISPLAY_MONITORING_WARNING) &&
				(getDisplayStatus() == DISPLAY_MONITORING_OK || getDisplayStatus() == DISPLAY_MONITORING_UNKNOWN || getDisplayStatus() == DISPLAY_MONITORING_WARNING) &&
				(getTempSensorStatus() == DISPLAY_MONITORING_ERROR || getTemperatureStatus() == DISPLAY_MONITORING_OK || getTemperatureStatus() == DISPLAY_MONITORING_UNKNOWN || getTemperatureStatus() == DISPLAY_MONITORING_WARNING) &&
				(getCommunicationStatus() == DISPLAY_MONITORING_OK || getCommunicationStatus() == DISPLAY_MONITORING_UNKNOWN || getCommunicationStatus() == DISPLAY_MONITORING_WARNING) &&
				(getLocalizationStatus() == DISPLAY_MONITORING_OK || getLocalizationStatus() == DISPLAY_MONITORING_UNKNOWN || getLocalizationStatus() == DISPLAY_MONITORING_WARNING)
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
			Status value
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



		Status DisplayMonitoringStatus::getLocalizationStatus(
		) const {
			return get<LocalizationStatus>();
		}



		void DisplayMonitoringStatus::setLocalizationStatus(
			Status value
		){
			set<LocalizationStatus>(value);
		}



		void DisplayMonitoringStatus::setScreen(
			const DisplayScreen* value
		) {
			_displayScreen = value;
			if (value)
			{
				set<StatusScreen>(value->getKey());
			}
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
			if (value)
			{
				set<StatusScreen>(value->getKey());
			}
		}

		void DisplayMonitoringStatus::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			RegistryKeyType id(get<StatusScreen>());
			try
			{
				setScreen(NULL);
				setCPU(NULL);
				if(decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					setScreen(DisplayScreenTableSync::Get(id, env).get());
				}
				else if(decodeTableId(id) == DisplayScreenCPUTableSync::TABLE.ID)
				{
					setCPU(DisplayScreenCPUTableSync::Get(id, env).get());
				}
				else
				{
					Log::GetInstance().warn("Data corrupted in "+ DisplayMonitoringStatus::TABLE_NAME + " on display screen : "+ lexical_cast<string>(id) + " not found");
				}
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				Log::GetInstance().warn("Data corrupted in "+ DisplayMonitoringStatus::TABLE_NAME + " on display screen : "+ lexical_cast<string>(id) + " not found");
			}
			catch (ObjectNotFoundException<DisplayScreenCPU>&)
			{
				Log::GetInstance().warn("Data corrupted in "+ DisplayMonitoringStatus::TABLE_NAME + " on display screen : "+ lexical_cast<string>(id) + " not found");
			}
		}

		boost::optional<double>	DisplayMonitoringStatus::getTemperatureValue()	const
		{
			if (get<TemperatureValue>() == 0.0)
			{
				return boost::optional<double>();
			}

			return boost::optional<double>(get<TemperatureValue>());
		}

		void DisplayMonitoringStatus::setTemperatureValue(boost::optional<double> value)
		{
			set<TemperatureValue>(value
				? *value
				: 0.0);
		}

		bool DisplayMonitoringStatus::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::WRITE);
		}

		bool DisplayMonitoringStatus::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::WRITE);
		}

		bool DisplayMonitoringStatus::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::DELETE_RIGHT);
		}
	}
}
