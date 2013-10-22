/**	ProjectAllPublicPlacesAction class implementation.
	@file ProjectAllPublicPlacesAction.cpp
	@date 2013

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ProjectAllPublicPlacesAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "PublicPlaceTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "RoadPlace.h"
#include "LexicalMatcher.h"
#include "DBTransaction.hpp"
#include "Road.h"
#include "RoadChunkTableSync.h"
#include "RequestException.h"

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
		template<> const string FactorableTemplate<Action, road::ProjectAllPublicPlacesAction>::FACTORY_KEY("ProjectAllPublicPlacesAction");
	}

	namespace road
	{
		const string ProjectAllPublicPlacesAction::PARAMETER_MAX_DISTANCE("md");
		const string ProjectAllPublicPlacesAction::PARAMETER_COMPATIBLE_USER_CLASSES_LIST("acList");


		ParametersMap ProjectAllPublicPlacesAction::getParametersMap() const
		{	
			ParametersMap map;
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			return map;
		}



		void ProjectAllPublicPlacesAction::_setFromParametersMap(const ParametersMap& map)
		{
			_maxDistance = map.getDefault<double>(PARAMETER_MAX_DISTANCE, 100);

			string userClassList(map.getDefault<string>(PARAMETER_COMPATIBLE_USER_CLASSES_LIST));
			try
			{
				if(!userClassList.empty())
				{
					vector<string> userClassVect;
					split(userClassVect, userClassList, is_any_of(",; "));
					BOOST_FOREACH(string& userClass, userClassVect)
					{
						UserClassCode code = lexical_cast<UserClassCode>(userClass);
						// Ignore invalid user class codes
						if(code > USER_CLASS_CODE_OFFSET && code < (USER_CLASS_CODE_OFFSET + USER_CLASSES_VECTOR_SIZE))
							_requiredUserClasses.insert(code);
					}		
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Bad user class code in acList parameter.");
			}
		}



		void ProjectAllPublicPlacesAction::run(
			Request& request
		){
			DBTransaction transaction;

			vector<boost::shared_ptr<PublicPlace> > publicPlaces(
				PublicPlaceTableSync::Search(
					Env::GetOfficialEnv(),
					optional<RegistryKeyType>(),
					optional<string>()
			)	);

			BOOST_FOREACH(const boost::shared_ptr<PublicPlace>& publicPlace, publicPlaces)
			{
				PublicPlaceEntrance entrance;
				entrance.set<PublicPlace>(*publicPlace);

				RoadChunkTableSync::ProjectAddress(
					*publicPlace->getPoint(),
					_maxDistance,
					entrance,
					_requiredUserClasses
				);

				// Saving
				PublicPlaceEntranceTableSync::Save(&entrance);

				// Object creation id
				if(request.getActionWillCreateObject())
				{
					request.setActionCreatedId(entrance.get<Key>());
				}
			}

			transaction.run();
		}



		bool ProjectAllPublicPlacesAction::isAuthorized(
			const Session* session
		) const {
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
			return true;
		}
}	}
