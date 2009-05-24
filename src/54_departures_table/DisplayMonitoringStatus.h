////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatus class header.
///	@file DisplayMonitoringStatus.h
///	@author Hugues Romain
///	@date 2008-12-19 0:41
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

#ifndef SYNTHESE_departurestable_DisplayMonitoringStatus_h__
#define SYNTHESE_departurestable_DisplayMonitoringStatus_h__

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;
		class DisplayScreenCPU;

		////////////////////////////////////////////////////////////////////////
		///	Display monitoring status class.
		///	
		///	
		///
		///	@ingroup m54
		////////////////////////////////////////////////////////////////////////
		class DisplayMonitoringStatus
		:	public util::Registrable
		{
		public:
			// Typedefs
			typedef enum
			{
				DISPLAY_MONITORING_OK		= 0,
				DISPLAY_MONITORING_WARNING	= 10,
				DISPLAY_MONITORING_ERROR	= 20,
				DISPLAY_MONITORING_UNKNOWN	= 30
			} Status;

			typedef util::Registry<DisplayMonitoringStatus> Registry;
			
		protected:
			// Attributes
			const DisplayScreen*		_displayScreen;
			const DisplayScreenCPU*		_cpu;
			boost::posix_time::ptime	_time;
			Status					_generalStatus;
			Status					_memoryStatus;
			Status					_clockStatus;
			Status					_eepromStatus;
			Status					_tempSensorStatus;
			Status					_lightStatus;
			std::string				_lightDetail;
			Status					_displayStatus;
			std::string				_displayDetail;
			Status					_soundStatus;
			std::string				_soundDetail;
			Status					_temperatureStatus;
			double					_temperatureValue;
			Status					_communicationStatus;
			Status					_localizationStatus;
			
		public:	
			////////////////////////////////////////////////////////////////////
			///	DisplayMonitoringStatus Constructor.
			///	@param id id of the object
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			DisplayMonitoringStatus(
				util::RegistryKeyType id = UNKNOWN_VALUE
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
				double				getTemperatureValue()		const;
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
				void	setTemperatureValue(double value);
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
			//@}
		};
	}
}

#endif // SYNTHESE_departurestable_DisplayMonitoringStatus_h__
