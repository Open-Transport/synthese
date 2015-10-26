
/** Depot class implementation.
	@file Depot.cpp

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

#include "Depot.hpp"

#include "ForbiddenUseRule.h"
#include "GlobalRight.h"
#include "Profile.h"
#include "PTOperationModule.hpp"
#include "Session.h"
#include "User.h"

#include <geos/geom/Point.h>

using namespace geos::geom;
using namespace boost;

namespace synthese
{
	using namespace graph;
	using namespace pt_operation;
	using namespace util;

	CLASS_DEFINITION(Depot, "t073_depots", 73)
	FIELD_DEFINITION_OF_OBJECT(Depot, "depot_id", "depot_ids")

	FIELD_DEFINITION_OF_TYPE(DepotDataSource, "datasource_links", SQL_TEXT)

	namespace pt_operation
	{
		Depot::Depot(RegistryKeyType id /*= 0*/ ):
			Registrable(id),
			Object<Depot, DepotSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(DepotDataSource),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)	),
			Vertex(NULL, boost::shared_ptr<geos::geom::Point>())
		{
			_hub = this;

			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			Hub::setRules(rules);
		}


		GraphIdType Depot::getGraphType() const
		{
			return PTOperationModule::GRAPH_ID;
		}


		boost::posix_time::time_duration Depot::getMinTransferDelay() const
		{
			return boost::posix_time::not_a_date_time;
		}

		void Depot::getVertexAccessMap(VertexAccessMap&, GraphIdType graphId, const Vertex &,bool) const
		{
		}

		bool Depot::isConnectionAllowed(const synthese::graph::Vertex &,const synthese::graph::Vertex &) const
		{
			return false;
		}

		boost::posix_time::time_duration Depot::getTransferDelay(const synthese::graph::Vertex &,const synthese::graph::Vertex &) const
		{
			return boost::posix_time::not_a_date_time;
		}

		HubScore Depot::getScore(void) const
		{
			return NO_TRANSFER_HUB_SCORE;
		}



		const boost::shared_ptr<Point>& Depot::getPoint() const
		{
			return getGeometry();
		}



		bool Depot::containsAnyVertex(synthese::graph::GraphIdType graphId) const
		{
			return graphId == PTOperationModule::GRAPH_ID;
		}



		std::string Depot::getRuleUserName() const
		{
			return "Dépôt";
		}



		Hub::Vertices Depot::getVertices( graph::GraphIdType graphId ) const
		{
			Vertices result;
			if(graphId == PTOperationModule::GRAPH_ID)
			{
				result.push_back(this);
			}
			return result;
		}

		bool Depot::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Depot::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool Depot::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
