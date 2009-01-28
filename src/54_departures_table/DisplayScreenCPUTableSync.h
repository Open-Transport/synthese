////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUTableSync class header.
///	@file DisplayScreenCPUTableSync.h
///	@author Hugues Romain
///	@date 2008-12-26 14:01
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

#ifndef SYNTHESE_DisplayScreenCPUTableSync_H__
#define SYNTHESE_DisplayScreenCPUTableSync_H__

#include "SQLiteRegistryTableSyncTemplate.h"

#include <vector>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreenCPU;

		////////////////////////////////////////////////////////////////////////
		/// DisplayScreenCPU table synchronizer class.
		///	@ingroup m54LS refLS
		////////////////////////////////////////////////////////////////////////
		class DisplayScreenCPUTableSync
		: public db::SQLiteRegistryTableSyncTemplate<DisplayScreenCPUTableSync,DisplayScreenCPU>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_PLACE_ID;
			static const std::string COL_MAC_ADDRESS;
			static const std::string COL_MONITORING_DELAY;
			static const std::string COL_IS_ONLINE;
			static const std::string COL_MAINTENANCE_MESSAGE;


		public:

			DisplayScreenCPUTableSync();


			////////////////////////////////////////////////////////////////////
			/// DisplayScreenCPU search.
			///	@param env Environment to populate
			///	@param first First DisplayScreenCPU object to answer
			///	@param number Number of DisplayScreenCPU objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			///	@param raisingOrder true = ascendant order, false = descendant order (default = true)
			///	@return vector<DisplayScreenCPU> Founded DisplayScreenCPU objects.
			///	@author Hugues Romain
			///	@date 2008
			static void Search(
				util::Env& env,
				util::RegistryKeyType placeId = UNKNOWN_VALUE,
				int first = 0,
				int number = 0,
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_DisplayScreenCPUTableSync_H__
