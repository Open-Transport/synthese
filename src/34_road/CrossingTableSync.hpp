
/** CrossingTableSync class header.
	@file CrossignTableSync.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "Address.h"
#include "FetcherTemplate.h"
#include "SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace road
	{
		/** 34.10 Crossing table synchronizer.
			@ingroup m34LS refLS

			Addresses table :
				- on insert : 
				- on update : 
				- on delete : X

			Table number : 2

			<h2>Fields</h2>

			<dl>
			<dt>id</dt><dd>primary key</dd>
			<dt>code_by_source</dt><dd>code of the address in the source database</dd>
			<dt>source_id</dt><dd>id of the @ref impex::DataSourceTableSync "source database" of the address</dd>
			<dt>longitude</dt><dd>longitude of the address in degrees</dd>
			<dt>latitude</dt><dd>latitude of the address in degrees</dd>
			</dl>
		*/
		class CrossingTableSync:
			public db::SQLiteRegistryTableSyncTemplate<CrossingTableSync,Address>,
			public db::FetcherTemplate<graph::Vertex, CrossingTableSync>
		{
		public:
			static const std::string COL_CODE_BY_SOURCE;
			static const std::string COL_SOURCE_ID;
			static const std::string COL_LONGITUDE;
			static const std::string COL_LATITUDE;
			
			CrossingTableSync();
			~CrossingTableSync();


			/** Address search.
				(other search parameters)
				@param first First Address object to answer
				@param number Number of Address objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Found Address objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

		};
	}
}

#endif // SYNTHESE_AddressTableSync_H__
