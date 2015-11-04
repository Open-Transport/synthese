////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatus class header.
///	@file DisplayMonitoringStatus.h
///	@author Hugues Romain
///	@date 2008-12-19 0:41
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

#ifndef SYNTHESE_departurestable_DisplayMonitoringStatus_h__
#define SYNTHESE_departurestable_DisplayMonitoringStatus_h__

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "PtimeField.hpp"
#include "StringField.hpp"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayScreenCPU;

		// Typedef
		typedef enum
		{
			DISPLAY_MONITORING_OK		= 0,
			DISPLAY_MONITORING_WARNING	= 10,
			DISPLAY_MONITORING_ERROR	= 20,
			DISPLAY_MONITORING_UNKNOWN	= 30
		} Status;

		FIELD_ID(StatusScreen)
		FIELD_PTIME(StatusTime)
		FIELD_ENUM(GeneralStatus, Status)
		FIELD_ENUM(MemoryStatus, Status)
		FIELD_ENUM(ClockStatus, Status)
		FIELD_ENUM(EepromStatus, Status)
		FIELD_ENUM(TempSensorStatus, Status)
		FIELD_ENUM(LightStatus, Status)
		FIELD_STRING(LightDetail)
		FIELD_ENUM(DisplayStatus, Status)
		FIELD_STRING(DisplayDetail)
		FIELD_ENUM(SoundStatus, Status)
		FIELD_STRING(SoundDetail)
		FIELD_ENUM(TemperatureStatus, Status)
		FIELD_DOUBLE(TemperatureValue)
		FIELD_ENUM(CommunicationStatus, Status)
		FIELD_ENUM(LocalizationStatus, Status)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(StatusScreen),
			FIELD(StatusTime),
			FIELD(GeneralStatus),
			FIELD(MemoryStatus),
			FIELD(ClockStatus),
			FIELD(EepromStatus),
			FIELD(TempSensorStatus),
			FIELD(LightStatus),
			FIELD(LightDetail),
			FIELD(DisplayStatus),
			FIELD(DisplayDetail),
			FIELD(SoundStatus),
			FIELD(SoundDetail),
			FIELD(TemperatureStatus),
			FIELD(TemperatureValue),
			FIELD(CommunicationStatus),
			FIELD(LocalizationStatus)
		> DisplayMonitoringStatusSchema;

		////////////////////////////////////////////////////////////////////////
		///	Display monitoring status class.
		///
		///
		///
		///	@ingroup m54
		////////////////////////////////////////////////////////////////////////
		class DisplayMonitoringStatus
		:	public Object<DisplayMonitoringStatus, DisplayMonitoringStatusSchema>
		{
		protected:
			// Attributes
			const DisplayScreen*		_displayScreen;
			const DisplayScreenCPU*		_cpu;

		public:
			////////////////////////////////////////////////////////////////////
			///	DisplayMonitoringStatus Constructor.
			///	@param id id of the object
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			DisplayMonitoringStatus(
				util::RegistryKeyType id = 0
			);



			////////////////////////////////////////////////////////////////////
			///	Constructor which parses the return of monitoring input
			///	interface page.
			///	@param monitoringInterfaceReturn the text returned by a
			///			ParseDisplayReturnInterfacePage object (fields separated
			///			by | character)
			/// @param displayScreen the monitored display screen
			///	@author Hugues Romain
			///	@date 2008
			///
			///	The time of the monitoring status is initialized at the current
			/// time at the object construction.
			///
			/// This method offers backward compatibility : it breaks at the
			/// last token even if all known fields are not fulfilled.
			/// Then old interfaces can be handled even if new fields have been
			/// added meanwhile.
			///
			/// An additional check is done by the constructor : the localization
			/// attribute of the display screen must not be NULL.
			////////////////////////////////////////////////////////////////////
			DisplayMonitoringStatus(
				const std::string& monitoringInterfaceReturn,
				const DisplayScreen* displayScreen
			);



			////////////////////////////////////////////////////////////////////
			///	Constructor for a CPU status.
			/// @param cpu the monitored display screen CPU
			///	@author Hugues Romain
			///	@date 2009
			///
			///	The time of the monitoring status is initialized at the current
			/// time at the object construction.
			///
			/// A display screen CPU is not properly monitored. The status
			/// is only used to show the last contact time.
			////////////////////////////////////////////////////////////////////
			DisplayMonitoringStatus(
				const DisplayScreenCPU* cpu
			);



			////////////////////////////////////////////////////////////////////
			///	DisplayMonitoringStatus Destructor.
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			~DisplayMonitoringStatus();

			//! @name Getters
			//@{
				Status				getGeneralStatus()			const;
				Status				getMemoryStatus()			const;
				Status				getClockStatus()			const;
				Status				getEepromStatus()			const;
				Status				getTempSensorStatus()		const;
				Status				getLightStatus()			const;
				const std::string&	getLightDetail()			const;
				Status				getDisplayStatus()			const;
				const std::string&	getDisplayDetail()			const;
				Status				getSoundStatus()			const;
				const std::string&	getSoundDetail()			const;
				Status				getTemperatureStatus()		const;
				boost::optional<double>	getTemperatureValue()	const;
				Status				getCommunicationStatus()	const;
				Status				getLocalizationStatus()		const;
				const boost::posix_time::ptime&	 getTime()		const;
				const DisplayScreen*	getScreen()				const;
				const DisplayScreenCPU*	getCPU()				const;
			//@}

			//! @name Setters
			//@{
				void	setGeneralStatus(Status value);
				void	setMemoryStatus(Status value);
				void	setClockStatus(Status value);
				void	setEepromStatus(Status value);
				void	setTempSensorStatus(Status value);
				void	setLightStatus(Status value);
				void	setLightDetail(const std::string& value);
				void	setDisplayStatus(Status value);
				void	setDisplayDetail(const std::string& value);
				void	setSoundStatus(Status value);
				void	setSoundDetail(const std::string& value);
				void	setTemperatureStatus(Status value);
				void	setTemperatureValue(boost::optional<double> value);
				void	setCommunicationStatus(Status value);
				void	setTime(const boost::posix_time::ptime& value);
				void	setLocalizationStatus(Status value);
				void	setScreen(const DisplayScreen* value);
				void	setCPU(const DisplayScreenCPU* value);
			//@}

			//! @name Queries
			//@{
				////////////////////////////////////////////////////////////////////
				///	Global status computing.
				///	@return DisplayMonitoringStatus::Status The global status of the screen
				///	@author Hugues Romain
				///	@date 2008
				///
				/// To obtain a OK global status :
				///		- general status must be OK
				///		- memory status must be OK or UNKNOWN
				///		- clock status must be OK or UNKNOWN
				///		- eeprom status must be OK or UNKNOWN
				///		- tempsensor status must be OK or UNKNOWN
				///		- light status must be OK or UNKNOWN
				///		- display status must be OK or UNKNOWN
				///		- sound status must be OK or UNKNOWN
				///		- temperature status must be OK or UNKNOWN
				///		- communication status must be OK or UNKNOWN
				///		- localization status must be OK or UNKNOWN
				///
				/// If the status is not OK :
				///	To obtain a WARNING global status :
				///		- general status must be OK or WARNING
				///		- memory status must be OK or WARNING or UNKNOWN
				///		- clock status must be OK or WARNING or ERROR or UNKNOWN
				///		- eeprom status must be OK or UNKNOWN
				///		- tempsensor status must be OK or WARNING or ERROR or UNKNOWN
				///		- light status must be OK or WARNING or ERROR or UNKNOWN
				///		- display status must be OK or WARNING or UNKNOWN
				///		- sound status must be OK or WARNING or ERROR or UNKNOWN
				///		- temperature status must be OK or WARNING or UNKNOWN (only if
				///			tempsensor is not ERROR)
				///		- communication status must be OK or WARNING or UNKNOWN
				///		- localization status must be OK or WARNING or UNKNOWN
				///
				/// If the status is not WARNING then it is ERROR
				////////////////////////////////////////////////////////////////////
				Status getGlobalStatus()	const;



				////////////////////////////////////////////////////////////////////
				///	Descriptive detail generator.
				///	@return std::string the summary of the status
				///	@author Hugues Romain
				///	@date 2008
				///
				///	Each following information is separated by / character :
				///		- Status : (OK|WARNING|ERROR)
				///		- If available, Temp : (temperature value)
				///		- For each not OK check : (name) : (WARNING|ERROR) + detail
				///			if available
				////////////////////////////////////////////////////////////////////
				std::string getDetail()	const;
			//@}

			//! @name Others
			//@{
				////////////////////////////////////////////////////////////////////
				///	Translates a status code into a string.
				///	@param value status code
				///	@return std::string OK/Warning/Error/Unknown
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				static std::string GetStatusString(Status value);



				////////////////////////////////////////////////////////////////////
				///	Translates a status code into a standardized icon.
				///	@param value status code
				///	@return std::string standardized icon file name
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				static std::string GetStatusIcon(Status value);

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_departurestable_DisplayMonitoringStatus_h__
