
////////////////////////////////////////////////////////////////////////////////
/// DisplayMaintenanceLog class header.
///	@file DisplayMaintenanceLog.h
///	@author Hugues Romain
///	@date 2008-12-26 21:39
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

#ifndef SYNTHESE_DISPLAY_MAINTENANCE_LOG
#define SYNTHESE_DISPLAY_MAINTENANCE_LOG

#include "DBLogTemplate.h"
#include "DBLogEntry.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayScreenCPU;
		class DisplayMonitoringStatus;

		////////////////////////////////////////////////////////////////////
		/// Displays maintenance log.
		///
		///	This log stores all the events concerning maintenance :
		///		- History of the user actions on the administrative console
		///		- History of monitoring status changes
		///		- History of detection of stop of monitoring
		///
		/// The DBLog fields are :
		///		- object : ID of the concerned display
		///		- level :
		///			- INFO : user action or monitoring status changing to OK value or restart of monitoring
		///			- WARNING : monitoring status changing to a WARNING value
		///			- ERROR : monitoring status changing to an ERROR value or detection of stop of monitoring
		///		- content :
		///			-# type of entry (DisplayMaintenanceLog::EntryType) :
		///				- DISPLAY_MAINTENANCE_ADMIN
		///				- DISPLAY_MONITORING_STATUS_CHANGE
		///				- DISPLAY_MONITORING_UP_DOWN_CHANGE
		///			-# Summary text
		///			-# Detailed text
		///		- user :
		///			- UNKNOWN_VALUE for status change entries
		///			- ID of the user who made the action for admin action entries
		////////////////////////////////////////////////////////////////////
		class DisplayMaintenanceLog:
			public dblog::DBLogTemplate<DisplayMaintenanceLog>
		{
			static const int _COL_TYPE;
			static const int _COL_FIELD;
			static const int _COL_TEXT;

		public:
			typedef enum
			{
				DISPLAY_MAINTENANCE_ADMIN = 20,
				DISPLAY_MONITORING_STATUS_CHANGE = 30,
				DISPLAY_MONITORING_UP = 40,
				DISPLAY_MONITORING_DOWN = 45
			} EntryType;

			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;



			DBLog::ColumnsVector parse(
				const dblog::DBLogEntry& entry,
				const server::Request& searchRequest
			) const;

			std::string getObjectName(
				util::RegistryKeyType id,
				const server::Request& searchRequest
			) const;



			////////////////////////////////////////////////////////////////////
			///	Entry creator for administration console uses logging.
			///	@param screen The edited screen
			///	@param user User who has made the edition
			///	@param field Edited field
			///	@param oldValue Old value of a field
			///	@param newValue New value of a field
			///	@param level Level of the entry (default Info)
			///	@author Hugues Romain
			///	@date 2008
			///
			/// The log entry is created only if oldValue and newValue are
			/// different.
			////////////////////////////////////////////////////////////////////
			static void	AddAdminEntry(
				const DisplayScreen& screen
				, const security::User& user
				, const std::string& field
				, const std::string& oldValue
				, const std::string& newValue
				, const dblog::Level level = dblog::DB_LOG_INFO
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for administration console uses logging.
			///	@param cpu The edited cpu
			///	@param user User who has made the edition
			///	@param field Edited field
			///	@param oldValue Old value of a field
			///	@param newValue New value of a field
			///	@param level Level of the entry (default Info)
			///	@author Hugues Romain
			///	@date 2008
			///
			/// The log entry is created only if oldValue and newValue are
			/// different.
			////////////////////////////////////////////////////////////////////
			static void	AddAdminEntry(
				const DisplayScreenCPU& cpu
				, const security::User& user
				, const std::string& field
				, const std::string& oldValue
				, const std::string& newValue
				, const dblog::Level level = dblog::DB_LOG_INFO
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for status change.
			///	@param screen The monitored screen
			///	@param oldValue old status value
			///	@param newValue new status value
			///	@author Hugues Romain
			///	@date 2008
			/// Level : generated from the new global status (see
			///	DisplayMonitoringStatus::getGlobalStatus() :
			///		- to OK status : Info
			///		- to WARNING or UNKNOWN status : Warning
			///		- to ERROR status : Error
			/// User : none
			///	Text : generated by DisplayMonitoringStatus::getDetail()
			////////////////////////////////////////////////////////////////////
			static void	AddStatusChangeEntry(
				const DisplayScreen& screen,
				const DisplayMonitoringStatus& oldValue,
				const DisplayMonitoringStatus& newValue
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for monitoring contact up.
			///	@param screen The monitored screen
			///	@param downTime time of the contact down
			///	@author Hugues Romain
			///	@date 2008
			/// Level : Info
			/// User : none
			/// Text : informations about duration of the contact loss period
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringUpEntry(
				const DisplayScreen& screen,
				const boost::posix_time::ptime& downTime
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for monitoring contact up.
			///	@param cpu The monitored cpu
			///	@param downTime time of the contact down
			///	@author Hugues Romain
			///	@date 2008
			/// Level : Info
			/// User : none
			/// Text : informations about duration of the contact loss period
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringUpEntry(
				const DisplayScreenCPU& cpu,
				const boost::posix_time::ptime& downTime
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for the first monitoring check.
			///	@param screen The monitored screen
			///	@param value Status of the display screen
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringFirstEntry(
				const DisplayScreen& screen,
				const DisplayMonitoringStatus& value
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for the first monitoring check.
			///	@param cpu The monitored cpu
			///	@param value Status of the display screen
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringFirstEntry(
				const DisplayScreenCPU& cpu,
				const DisplayMonitoringStatus& value
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for monitoring contact down.
			///	@param screen The monitored screen
			///	@author Hugues Romain
			///	@date 2008
			/// Level : Error
			/// User : none
			/// Text : nothing
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringDownEntry(
				const DisplayScreen& screen
			);



			////////////////////////////////////////////////////////////////////
			///	Entry creator for monitoring contact down.
			///	@param cpu The monitored cpu
			///	@author Hugues Romain
			///	@date 2008
			/// Level : Error
			/// User : none
			/// Text : nothing
			////////////////////////////////////////////////////////////////////
			static void AddMonitoringDownEntry(
				const DisplayScreenCPU& cpu
			);
		};
	}
}

#endif

