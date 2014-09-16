
/** CrossingTableSync class header.
	@file CrossingTableSync.hpp
	@author Hugues Romain

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

#ifndef SYNTHESE_CrossingTableSync_H__
#define SYNTHESE_CrossingTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "Crossing.h"
#include "FetcherTemplate.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace road
	{
		/** 34.10 Crossing table synchronizer.
			@ingroup m34LS refLS
			@author Marc Jambert, Hugues Romain
			@date 2006

			Table number : 43
			Corresponding class : Crossing

			<h2>Fields</h2>

			<dl>
			<dt>id</dt><dd>primary key</dd>
			<dt>code_by_source</dt><dd>code of the address in the source database</dd>
			<dt>longitude</dt><dd>longitude of the address in degrees</dd>
			<dt>latitude</dt><dd>latitude of the address in degrees</dd>
			</dl>

			<h2>Up links</h2>

			<dl>
			<dt>source_id</dt><dd>id of the @ref impex::DataSourceTableSync "source database" of the crossing</dd>
			</dl>
		*/
		class CrossingTableSync:
			public db::DBDirectTableSyncTemplate<
				CrossingTableSync,
				Crossing
			>,
			public db::FetcherTemplate<graph::Vertex, CrossingTableSync>
		{
		public:

			CrossingTableSync() {}
			~CrossingTableSync() {}


			//////////////////////////////////////////////////////////////////////////
			/// Crossings search.
			///	@param env Environment to populate when loading objects
			///	@param first First Crossing object to answer
			///	@param number Number of Crossing objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			/// @param linkLevel level of link
			///	@return Found Crossing objects.
			///	@author Hugues Romain
			///	@date 2006
			static SearchResult Search(
				util::Env& env,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			static std::string SerializeNonReachableRoads(
				const Crossing::NonReachableRoadFromRoad& value
			);

			static Crossing::NonReachableRoadFromRoad UnserializeNonReachableRoads(
				const std::string& value,
				util::Env& env
			);
		};
	}
}

#endif // SYNTHESE_AddressTableSync_H__
