
//////////////////////////////////////////////////////////////////////////
/// ProjectAllStopPointsAction class implementation.
/// @file ProjectAllStopPointsAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ProjectAllStopPointsAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "RoadPlace.h"
#include "LexicalMatcher.h"
#include "DBTransaction.hpp"
#include "StopArea.hpp"
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
		template<> const string FactorableTemplate<Action, pt::ProjectAllStopPointsAction>::FACTORY_KEY("ProjectAllStopPointsAction");
	}

	namespace pt
	{
		const string ProjectAllStopPointsAction::PARAMETER_MAX_DISTANCE("md");
		const string ProjectAllStopPointsAction::PARAMETER_COMPATIBLE_USER_CLASSES_LIST("acList");


		ParametersMap ProjectAllStopPointsAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			return map;
		}



		void ProjectAllStopPointsAction::_setFromParametersMap(const ParametersMap& map)
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



		void ProjectAllStopPointsAction::run(
			Request& request
		){
			DBTransaction transaction;

			vector<boost::shared_ptr<StopPoint> > stopPoints(
				StopPointTableSync::Search(
					Env::GetOfficialEnv(),
					optional<RegistryKeyType>(),
					optional<string>(),
					true,
					true
			)	);

			BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stopPoint, stopPoints)
			{
				Address address;
				if(stopPoint->hasGeometry())
				{
					RoadChunkTableSync::ProjectAddress(
						*stopPoint->getGeometry(),
						_maxDistance,
						address,
						_requiredUserClasses
					);
				}
				if(address.getRoadChunk())
				{
					stopPoint->setProjectedPoint(address);
					StopPointTableSync::Save(stopPoint.get(), transaction);
				}
			}

			transaction.run();
		}



		bool ProjectAllStopPointsAction::isAuthorized(
			const Session* session
		) const {
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
			return true;
		}
}	}
