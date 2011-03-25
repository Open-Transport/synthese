
//////////////////////////////////////////////////////////////////////////
/// TransportNetworkUpdateAction class implementation.
/// @file TransportNetworkUpdateAction.cpp
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
#include "TransportNetworkUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "ImportableTableSync.hpp"
#include "ImportableAdmin.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace impex;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::TransportNetworkUpdateAction>::FACTORY_KEY("network_update");
	}

	namespace pt
	{
		const string TransportNetworkUpdateAction::PARAMETER_NETWORK_ID = Action_PARAMETER_PREFIX + "ni";
		const string TransportNetworkUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		
		
		
		ParametersMap TransportNetworkUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_network.get() && _network->getKey())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			return map;
		}
		
		
		
		void TransportNetworkUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getOptional<RegistryKeyType>(PARAMETER_NETWORK_ID)) try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), *_env);
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw ActionException("No such network");
			}
			else
			{
				_network.reset(new TransportNetwork);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
				if (_name->empty())
				{
					throw ActionException("Le nom du réseau ne peut pas être vide.");	
				}
				
				TransportNetworkTableSync::SearchResult networks(
					TransportNetworkTableSync::Search(*_env, *_name, string(), 0, 1)
				);
				if(!networks.empty() && (networks.size() > 1 || networks.front() == _network))
				{
					throw ActionException("Un réseau nommé "+ *_name +" existe déjà.");
				}
			}

			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS), *_env);
			}
		}
		
		
		
		void TransportNetworkUpdateAction::run(
			Request& request
		){
			if(_name)
			{
				_network->setName(*_name);
			}
			if(_dataSourceLinks)
			{
				_network->setDataSourceLinks(*_dataSourceLinks);
			}

			TransportNetworkTableSync::Save(_network.get());
//			::AddCreationEntry(object, request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_network->getKey());
			}
		}
		
		
		
		bool TransportNetworkUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
