
//////////////////////////////////////////////////////////////////////////
/// CleanAllStopPointProjectionsAction class implementation.
/// @file CleanAllStopPointProjectionsAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "CleanAllStopPointProjectionsAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "Address.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace road;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::CleanAllStopPointProjectionsAction>::FACTORY_KEY("CleanAllStopPointProjections");
	}

	namespace pt
	{
		const string CleanAllStopPointProjectionsAction::PARAMETER_AUTOMATED_ONLY = Action_PARAMETER_PREFIX + "_automated_only";



		ParametersMap CleanAllStopPointProjectionsAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_AUTOMATED_ONLY, _automatedOnly);
			return map;
		}



		void CleanAllStopPointProjectionsAction::_setFromParametersMap(const ParametersMap& map)
		{
			_automatedOnly = map.getDefault<bool>(PARAMETER_AUTOMATED_ONLY, true);
		}



		void CleanAllStopPointProjectionsAction::run(
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
				// Todo
				if(_automatedOnly && false)
				{
					continue;
				}

				road::Address emptyAddress;
				stopPoint->setProjectedPoint(emptyAddress);
				StopPointTableSync::Save(stopPoint.get(), transaction);
			}

			transaction.run();
		}



		bool CleanAllStopPointProjectionsAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		CleanAllStopPointProjectionsAction::CleanAllStopPointProjectionsAction():
		_automatedOnly(true)
		{}
}	}
