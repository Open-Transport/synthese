
//////////////////////////////////////////////////////////////////////////
/// CommercialLineAddAction class implementation.
/// @file CommercialLineAddAction.cpp
/// @author Hugues
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
#include "CommercialLineAddAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "TransportNetwork.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::CommercialLineAddAction>::FACTORY_KEY("CommercialLineAddAction");
	}

	namespace pt
	{
		const string CommercialLineAddAction::PARAMETER_NETWORK_ID = Action_PARAMETER_PREFIX + "ni";
		
		
		
		ParametersMap CommercialLineAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return map;
		}
		
		
		
		void CommercialLineAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_network = Env::GetOfficialEnv().get<TransportNetwork>(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw ActionException("No such network");
			}

		}
		
		
		
		void CommercialLineAddAction::run(
			Request& request
		){
			CommercialLine line;
			line.setNetwork(_network.get());
			CommercialLineTableSync::Save(&line);
			//::AddCreationEntry(object, request.getUser().get());

			request.setActionCreatedId(line.getKey());
		}
		
		
		
		bool CommercialLineAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		void CommercialLineAddAction::setNetwork( boost::shared_ptr<const TransportNetwork> value )
		{
			_network = value;
		}
	}
}
