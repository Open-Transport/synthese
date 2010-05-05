
//////////////////////////////////////////////////////////////////////////
/// LineStopRemoveAction class implementation.
/// @file LineStopRemoveAction.cpp
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
#include "LineStopRemoveAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "LineStopTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::LineStopRemoveAction>::FACTORY_KEY("LineStopRemoveAction");
	}

	namespace pt
	{
		const string LineStopRemoveAction::PARAMETER_LINESTOP_ID = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap LineStopRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_lineStop.get())
			{
				map.insert(PARAMETER_LINESTOP_ID, _lineStop->getKey());
			}
			return map;
		}
		
		
		
		void LineStopRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_lineStop = Env::GetOfficialEnv().get<LineStop>(map.get<RegistryKeyType>(PARAMETER_LINESTOP_ID));
			}
			catch(ObjectNotFoundException<LineStop>&)
			{
				throw ActionException("No such line stop");
			}
		}
		
		
		
		void LineStopRemoveAction::run(
			Request& request
		){
			LineStopTableSync::RemoveStop(*_lineStop);

//			::AddDeleteEntry(*_object, request.getUser().get());
		}
		
		
		
		bool LineStopRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
