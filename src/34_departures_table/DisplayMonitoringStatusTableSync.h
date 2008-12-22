////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatusTableSync class header.
///	@file DisplayMonitoringStatusTableSync.h
///	@author Hugues Romain
///	@date 2008-12-19 10:49
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

#ifndef SYNTHESE_DisplayMonitoringStatusTableSync_H__
#define SYNTHESE_DisplayMonitoringStatusTableSync_H__

// Std
#include <vector>
#include <string>
#include <iostream>

// Db
#include "SQLiteNoSyncTableSyncTemplate.h"

// Boost
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace departurestable
	{
		class DisplayMonitoringStatus;

		////////////////////////////////////////////////////////////////////////
		/// DisplayMonitoringStatus table synchronizer class.
		///	@ingroup m54LS refLS
		////////////////////////////////////////////////////////////////////////
		class DisplayMonitoringStatusTableSync
		: public db::SQLiteNoSyncTableSyncTemplate<DisplayMonitoringStatusTableSync,DisplayMonitoringStatus>
		{
		public:
			static const std::string COL_SCREEN_ID;
			static const std::string COL_TIME;
			static const std::string COL_GENERAL_STATUS;
			static const std::string COL_MEMORY_STATUS;
			static const std::string COL_CLOCK_STATUS;
			static const std::string COL_EEPROM_STATUS;
			static const std::string COL_TEMP_SENSOR_STATUS;
			static const std::string COL_LIGHT_STATUS;
			static const std::string COL_LIGHT_DETAIL;
			static const std::string COL_DISPLAY_STATUS;
			static const std::string COL_DISPLAY_DETAIL;
			static const std::string COL_SOUND_STATUS;
			static const std::string COL_SOUND_DETAIL;
			static const std::string COL_TEMPERATURE_STATUS;
			static const std::string COL_TEMPERATURE_VALUE;
			static const std::string COL_COMMUNICATION_STATUS;
			static const std::string COL_LOCALIZATION_STATUS;
			
			DisplayMonitoringStatusTableSync();


			/** DisplayMonitoringStatus search.
				(other search parameters)
				@param first First DisplayMonitoringStatus object to answer
				@param number Number of DisplayMonitoringStatus objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param raisingOrder true = ascendant order, false = descendant order (default = true)
				@return vector<DisplayMonitoringStatus> Founded DisplayMonitoringStatus objects.
				@author Hugues Romain
				@date 2006
			*/
			static void Search(
				util::Env& env,
				util::RegistryKeyType screenId = UNKNOWN_VALUE,
				int first = 0,
				int number = 0,
				bool orderByScreenId = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			
			////////////////////////////////////////////////////////////////////
			///	Gets the monitoring status of a screen.
			///	@param screenId id of the screen to describe
			///	@return Pointer to the corresponding status
			/// @warning The returned object is incomplete : the _screen pointer
			///		has NULL value. Use it to read the monitored properties
			///		only
			///	@author Hugues Romain
			///	@date 2008
			///	If the screen has never sent monitoring messages, then a
			///	UNKWNOWN status is returned (no exception)
			////////////////////////////////////////////////////////////////////
			static boost::shared_ptr<DisplayMonitoringStatus> GetStatus(
				util::RegistryKeyType screenId
			);
		};
	}
}

#endif // SYNTHESE_DisplayMonitoringStatusTableSync_H__
