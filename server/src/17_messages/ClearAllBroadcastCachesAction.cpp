
//////////////////////////////////////////////////////////////////////////
/// ClearAllBroadcastCachesAction class implementation.
/// @file ClearAllBroadcastCachesAction.cpp
/// @author hromain
/// @date 2013
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

#include "ClearAllBroadcastCachesAction.hpp"

#include "ActionException.h"
#include "MessagesModule.h"
#include "ParametersMap.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, messages::ClearAllBroadcastCachesAction>::FACTORY_KEY = "clear_all_broadcast_cache";

	namespace messages
	{
		ParametersMap ClearAllBroadcastCachesAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void ClearAllBroadcastCachesAction::_setFromParametersMap(const ParametersMap& map)
		{
		}
		
		
		
		void ClearAllBroadcastCachesAction::run(
			Request& request
		){
			MessagesModule::ClearAllBroadcastCaches();
		}
		
		
		
		bool ClearAllBroadcastCachesAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
}	}

