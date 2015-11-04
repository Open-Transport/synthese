////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatusTableSync class header.
///	@file DisplayMonitoringStatusTableSync.h
///	@author Hugues Romain
///	@date 2008-12-19 10:49
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

#ifndef SYNTHESE_DisplayMonitoringStatusTableSync_H__
#define SYNTHESE_DisplayMonitoringStatusTableSync_H__

#include "DisplayMonitoringStatus.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "NoSynchronizationPolicy.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayScreenCPU;

		////////////////////////////////////////////////////////////////////////
		/// DisplayMonitoringStatus table synchronizer class.
		///	@ingroup m54LS refLS
		////////////////////////////////////////////////////////////////////////
		class DisplayMonitoringStatusTableSync:
			public db::DBDirectTableSyncTemplate<
				DisplayMonitoringStatusTableSync,
				DisplayMonitoringStatus,
				db::NoSynchronizationPolicy
			>
		{
		public:
			/** DisplayMonitoringStatus search.
				(other search parameters)
				@param first First DisplayMonitoringStatus object to answer
				@param number Number of DisplayMonitoringStatus objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param raisingOrder true = ascendant order, false = descendant order (default = true)
				@return vector<DisplayMonitoringStatus> Founded DisplayMonitoringStatus objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> screenId = boost::optional<util::RegistryKeyType>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByScreenId = true,
				bool orderByTime = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			////////////////////////////////////////////////////////////////////
			///	Gets the monitoring status of a screen.
			///	@param screen the screen to describe
			///	@return Pointer to the corresponding status
			/// @warning The returned object is incomplete : the _screen pointer
			///		has NULL value. Use it to read the monitored properties
			///		only
			///	@author Hugues Romain
			///	@date 2008
			///	If the screen has never sent monitoring messages, then a
			///	UNKWNOWN status is returned (no exception)
			/// Writes a log if the screen is down according to the delay
			/// between checks
			////////////////////////////////////////////////////////////////////
			static boost::shared_ptr<DisplayMonitoringStatus> GetStatus(
				const DisplayScreen& screen
			);



			////////////////////////////////////////////////////////////////////
			///	Gets the last contact time of a screen CPU.
			///	@param cpu the cpu to read
			///	@return time of the last contact with the CPU
			///	@author Hugues Romain
			///	@date 2009
			///	If the cpu has never sent monitoring messages, then a
			///	not_a_date_time value is returned (no exception)
			/// Writes a log if the cpu is down according to the delay
			/// between checks
			////////////////////////////////////////////////////////////////////
			static boost::posix_time::ptime GetLastContact(
				const DisplayScreenCPU& cpu
			);


			static boost::shared_ptr<DisplayMonitoringStatus> UpdateStatus(
				util::Env& env,
				const DisplayScreen& screen,
				bool archive
			);

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_DisplayMonitoringStatusTableSync_H__
