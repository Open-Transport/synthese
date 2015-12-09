
//////////////////////////////////////////////////////////////////////////
/// ProjectPublicBikeStationsAction class implementation.
/// @file ProjectPublicBikeStationsAction.cpp
/// @author Camille Hue
/// @date 2015
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ProjectPublicBikeStations.hpp"

#include "PublicBikeStation.hpp"
#include "PublicBikeStationTableSync.hpp"
#include "RoadChunkTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		template<> const string FactorableTemplate<server::Action, public_biking::ProjectPublicBikeStationsAction>::FACTORY_KEY("ProjectPublicBikeStationsAction");
	}

	namespace public_biking
	{
		const string ProjectPublicBikeStationsAction::PARAMETER_MAX_DISTANCE("md");


		util::ParametersMap ProjectPublicBikeStationsAction::getParametersMap() const
		{
			util::ParametersMap map;
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			return map;
		}



		void ProjectPublicBikeStationsAction::_setFromParametersMap(const util::ParametersMap& map)
		{
			_maxDistance = map.getDefault<double>(PARAMETER_MAX_DISTANCE, 100);
		}



		void ProjectPublicBikeStationsAction::run(
			server::Request& request
		){
			db::DBTransaction transaction;

			vector<boost::shared_ptr<PublicBikeStation> > publicBikeStations(
				PublicBikeStationTableSync::Search(
					util::Env::GetOfficialEnv(),
					optional<util::RegistryKeyType>()
			)	);

			BOOST_FOREACH(const boost::shared_ptr<PublicBikeStation>& publicBikeStation, publicBikeStations)
			{
				road::Address address;
				if(publicBikeStation->hasGeometry())
				{
					road::RoadChunkTableSync::ProjectAddress(
						*publicBikeStation->getGeometry(),
						_maxDistance,
						address
					);
				}
				if(address.getRoadChunk())
				{
					publicBikeStation->setProjectedPoint(address);
					PublicBikeStationTableSync::Save(publicBikeStation.get(), transaction);
				}
			}

			transaction.run();
		}



		bool ProjectPublicBikeStationsAction::isAuthorized(
			const server::Session* session
		) const {
			return true;
		}
}	}
