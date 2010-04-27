
//////////////////////////////////////////////////////////////////////////
/// HikingTrailRemoveAction class implementation.
/// @file HikingTrailRemoveAction.cpp
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
#include "HikingTrailRemoveAction.h"
#include "HikingRight.h"
#include "Request.h"
#include "HikingTrailTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, hiking::HikingTrailRemoveAction>::FACTORY_KEY("HikingTrailRemoveAction");
	}

	namespace hiking
	{
		const string HikingTrailRemoveAction::PARAMETER_TRAIL_ID = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap HikingTrailRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_trail.get())
			{
				map.insert(PARAMETER_TRAIL_ID, _trail->getKey());
			}
			return map;
		}
		
		
		
		void HikingTrailRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_trail = Env::GetOfficialEnv().get<HikingTrail>(map.get<RegistryKeyType>(PARAMETER_TRAIL_ID));
			}
			catch(ObjectNotFoundException<HikingTrail>&)
			{
				throw ActionException("No such trail");
			}

		}
		
		
		
		void HikingTrailRemoveAction::run(
			Request& request
		){
			HikingTrailTableSync::Remove(_trail->getKey());
//			::AddDeleteEntry(*_object, request.getUser().get());
		}
		
		
		
		bool HikingTrailRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(DELETE_RIGHT);
		}
	}
}
