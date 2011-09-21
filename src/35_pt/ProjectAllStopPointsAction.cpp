
//////////////////////////////////////////////////////////////////////////
/// ProjectAllStopPointsAction class implementation.
/// @file ProjectAllStopPointsAction.cpp
/// @author Hugues Romain
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "ProjectAllStopPointsAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "EdgeProjector.hpp"
#include "RoadPlace.h"
#include "LexicalMatcher.h"
#include "DBTransaction.hpp"
#include "StopArea.hpp"
#include "Road.h"
#include "RoadChunkTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace algorithm;
	using namespace geography;
	using namespace road;
	using namespace db;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ProjectAllStopPointsAction>::FACTORY_KEY("ProjectAllStopPointsAction");
	}

	namespace pt
	{
		const string ProjectAllStopPointsAction::PARAMETER_MAX_DISTANCE("md");


		ParametersMap ProjectAllStopPointsAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			return map;
		}



		void ProjectAllStopPointsAction::_setFromParametersMap(const ParametersMap& map)
		{
			_maxDistance = map.getDefault<double>(PARAMETER_MAX_DISTANCE, 100);
		}



		void ProjectAllStopPointsAction::run(
			Request& request
		){
			DBTransaction transaction;

			vector<shared_ptr<StopPoint> > stopPoints(
				StopPointTableSync::Search(
					Env::GetOfficialEnv(),
					optional<RegistryKeyType>(),
					optional<string>(),
					true,
					true
			)	);


			BOOST_FOREACH(shared_ptr<StopPoint> stopPoint, stopPoints)
			{
				Address emptyAddress;
				stopPoint->setProjectedPoint(emptyAddress);

				if(stopPoint->hasGeometry())
				{

					EdgeProjector<shared_ptr<MainRoadChunk> >::From paths(
						RoadChunkTableSync::SearchByMaxDistance(
							*stopPoint->getGeometry(), _maxDistance,
							Env::GetOfficialEnv(), UP_LINKS_LOAD_LEVEL
					)	);

					if(!paths.empty())
					{

						EdgeProjector<shared_ptr<MainRoadChunk> > projector(paths, _maxDistance);

						try
						{
							EdgeProjector<shared_ptr<MainRoadChunk> >::PathNearby projection(projector.projectEdge(*stopPoint->getGeometry()->getCoordinate()));

							Address projectedAddress(
								*projection.get<1>(),
								projection.get<2>()
							);
							stopPoint->setProjectedPoint(projectedAddress);
						}
						catch(EdgeProjector<shared_ptr<MainRoadChunk> >::NotFoundException)
						{
						}
					}
				}
				StopPointTableSync::Save(stopPoint.get(), transaction);
			}

			transaction.run();
		}



		bool ProjectAllStopPointsAction::isAuthorized(
			const Session* session
		) const {
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
			return true;
		}
	}
}
