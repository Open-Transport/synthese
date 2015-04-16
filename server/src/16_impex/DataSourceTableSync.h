////////////////////////////////////////////////////////////////////////////////
///	DataSource Table synchronizer class header.
///	@file DataSourceTableSync.h
///	@author Hugues Romain
///	@date 2009
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

#ifndef SYNTHESE_impex_DataSourceTableSync_H
#define SYNTHESE_impex_DataSourceTableSync_H

#include <string>
#include <iostream>

#include "DataSource.h"

#include "DBDirectTableSyncTemplate.hpp"
#include "StandardLoadSavePolicy.hpp"

namespace synthese
{
	namespace impex
	{
		/** DataSource table synchronizer.
			Automatic sychrnozation : none
			@ingroup m16LS refLS
		*/
		class DataSourceTableSync:
			public db::DBDirectTableSyncTemplate<
				DataSourceTableSync,
				DataSource,
				db::FullSynchronizationPolicy,
				db::StandardLoadSavePolicy
			>
		{
		public:

			DataSourceTableSync() {}

			/** DataSource search.
				@param env Environment to read and populate
				@param name Filter on a part of the name of the data source
				@param format Filter on the format of the data source
				@param first First JourneyPattern object to answer
				@param number Number of JourneyPattern objects to answer (0 = all) The size of the vector is less
					or equal to number, then all users were returned despite of the number limit. If the
					size is greater than number (actually equal to number + 1) then there is others accounts
					to show. Test it to know if the situation needs a "click for more" button.
				@param orderByName Order by the name field
				@param raisingOrder direction of ordering (true = ascendent order, false = descendent order)
				@param linkLevel level of recursion of object creation and link
				@author Hugues Romain
				@date 2007
			*/
			static SearchResult Search(
				util::Env& env,
				std::string name = std::string(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif
