
/** HouseTableSync class implementation.
	@file HouseTableSync.cpp

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

#include "HouseTableSync.hpp"

#include "EdgeProjector.hpp"
#include "House.hpp"
#include "RoadChunk.h"
#include "DBModule.h"
#include "LinkException.h"
#include "DataSource.h"
#include "DataSourceLinksField.hpp"
#include "DataSourceTableSync.h"
#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "ImportableTableSync.hpp"
#include "Road.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"

#include <sstream>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, HouseTableSync>::FACTORY_KEY("34.40.01 House");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<HouseTableSync>::TABLE(
			"t078_houses"
		);

		template<> const Field DBTableSyncTemplate<HouseTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<HouseTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}

		template<> bool DBTableSyncTemplate<HouseTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HouseTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}

	}

	namespace road
	{

		bool HouseTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

	}
}
