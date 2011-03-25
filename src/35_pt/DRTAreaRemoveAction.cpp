
//////////////////////////////////////////////////////////////////////////
/// DRTAreaRemoveAction class implementation.
/// @file DRTAreaRemoveAction.cpp
/// @author RCSobility
/// @date 2011
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
#include "DRTAreaRemoveAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "DRTAreaTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::DRTAreaRemoveAction>::FACTORY_KEY("DRTAreaRemoveAction");
	}

	namespace pt
	{
		const string DRTAreaRemoveAction::PARAMETER_AREA = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap DRTAreaRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_area.get())
			{
				map.insert(PARAMETER_AREA, _area->getKey());
			}
			return map;
		}
		
		
		
		void DRTAreaRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_area = DRTAreaTableSync::Get(map.get<RegistryKeyType>(PARAMETER_AREA), *_env);
			}
			catch(ObjectNotFoundException<DRTArea>&)
			{
				throw ActionException("No such area");
			}

		}
		
		
		
		void DRTAreaRemoveAction::run(
			Request& request
		){
			DRTAreaTableSync::Remove(_area->getKey());
			//::AddDeleteEntry(*_object, *request.getUser());
		}
		
		
		
		bool DRTAreaRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}
	}
}
