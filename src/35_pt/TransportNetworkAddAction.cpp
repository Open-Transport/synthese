
//////////////////////////////////////////////////////////////////////////
/// TransportNetworkAddAction class implementation.
/// @file TransportNetworkAddAction.cpp
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
#include "TransportNetworkAddAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::TransportNetworkAddAction>::FACTORY_KEY("TransportNetworkAddAction");
	}

	namespace pt
	{
		const string TransportNetworkAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		
		
		
		ParametersMap TransportNetworkAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void TransportNetworkAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.get<string>(PARAMETER_NAME);
			if (_name.empty())
			{
				throw ActionException("Le nom du réseau ne peut pas être vide.");	
			}

			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(*_env, _name, string(), 0, 1)
			);
			if(!networks.empty())
			{
				throw ActionException("Un réseau nommé "+ _name +" existe déjà.");
			}
		}
		
		
		
		void TransportNetworkAddAction::run(
			Request& request
		){
			TransportNetwork object;
			object.setName(_name);
			TransportNetworkTableSync::Save(&object);
//			::AddCreationEntry(object, request.getUser().get());

			request.setActionCreatedId(object.getKey());
		}
		
		
		
		bool TransportNetworkAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
