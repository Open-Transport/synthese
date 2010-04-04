
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleAddAction class implementation.
/// @file PTUseRuleAddAction.cpp
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
#include "PTUseRuleAddAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "PTUseRuleTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::PTUseRuleAddAction>::FACTORY_KEY("PTUseRuleAddAction");
	}

	namespace pt
	{
		const string PTUseRuleAddAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";
		const string PTUseRuleAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";

		
		
		ParametersMap PTUseRuleAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_template.get())
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
			}
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void PTUseRuleAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getDefault<string>(PARAMETER_NAME);
			
			// Template
			RegistryKeyType tid(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0));
			if(tid > 0)	try
			{
				_template = PTUseRuleTableSync::Get(tid, *_env);
			}
			catch(ObjectNotFoundException<PTUseRule>&)
			{
				throw ActionException("No such template");
			}

		}
		
		
		
		void PTUseRuleAddAction::run(
			Request& request
		){
			PTUseRule object;
			object.setName(_name);

			if(_template.get())
			{
				object.setAccessCapacity(_template->getAccessCapacity());
				object.setDefaultFare(_template->getDefaultFare());
				object.setHourDeadLine(_template->getHourDeadLine());
				object.setMaxDelayDays(_template->getMaxDelayDays());
				object.setMinDelayDays(_template->getMinDelayDays());
				object.setMinDelayMinutes(_template->getMinDelayMinutes());
				object.setOriginIsReference(_template->getOriginIsReference());
				object.setReservationType(_template->getReservationType());
			}

			PTUseRuleTableSync::Save(&object);
			
			//::AddCreationEntry(object, request.getUser().get());

			request.setActionCreatedId(object.getKey());
		}
		
		
		
		bool PTUseRuleAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
