
/** RoadChunkTableSync class implementation.
	@file RoadChunkTableSync.cpp

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

#include "RoadChunkTableSync.h"

#include "Address.h"
#include "CrossingTableSync.hpp"
#include "RoadModule.h"
#include "RoadTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "LinkException.h"
#include "CoordinatesSystem.hpp"
#include "RuleUser.h"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"
#include "AccessParameters.h"
#include "RoadChunkEdge.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/LineString.h>


using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace util;
	using namespace road;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, RoadChunkTableSync>::FACTORY_KEY("34.30.01 Road chunks");
	}

	namespace road
	{
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<RoadChunkTableSync>::TABLE(
			"t014_road_chunks"
		);

		template<> const Field DBTableSyncTemplate<RoadChunkTableSync>::_FIELDS[]=
		{
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<RoadChunkTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Road::FIELD.name.c_str(),
					RankInPathField::FIELD.name.c_str(),
					""
			)	);
			return r;
		}


		template<> bool DBTableSyncTemplate<RoadChunkTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<RoadChunkTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace road
	{

		bool RoadChunkTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

	    RoadChunkTableSync::SearchResult RoadChunkTableSync::Search(
			Env& env,
			optional<RegistryKeyType> roadId,
			int first /*= 0*/,
			boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			SelectQuery<RoadChunkTableSync> query;
			if(roadId)
			{
				query.addWhereField(Road::FIELD.name, *roadId);
			}
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
	    }



		void RoadChunkTableSync::ProjectAddress(
			const Point& point,
			double maxDistance,
			Address& address,
			EdgeProjector<boost::shared_ptr<RoadChunk> >::CompatibleUserClassesRequired requiredUserClasses
		){
			EdgeProjector<boost::shared_ptr<RoadChunk> >::From paths(
				SearchByMaxDistance(
					point,
					maxDistance,
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
			)	);

			if(!paths.empty())
			{
				EdgeProjector<boost::shared_ptr<RoadChunk> > projector(paths, maxDistance, requiredUserClasses);

				try
				{
					EdgeProjector<boost::shared_ptr<RoadChunk> >::PathNearby projection(
						projector.projectEdge(
							*point.getCoordinate()
					)	);

					address.setGeometry(
						boost::shared_ptr<Point>(
							CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createPoint(
								projection.get<0>()
					)	)	);
					address.setRoadChunk(projection.get<1>().get());
					address.setMetricOffset(projection.get<2>());
				}
				catch(EdgeProjector<boost::shared_ptr<RoadChunk> >::NotFoundException)
				{
				}
			}
		}
}	}
