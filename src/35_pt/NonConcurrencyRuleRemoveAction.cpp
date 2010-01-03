
/** NonConcurrencyRuleRemoveAction class implementation.
	@file NonConcurrencyRuleRemoveAction.cpp
	@author Hugues
	@date 2009

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ActionException.h"
#include "ParametersMap.h"
#include "NonConcurrencyRuleRemoveAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace env;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::NonConcurrencyRuleRemoveAction>::FACTORY_KEY("NonConcurrencyRuleRemoveAction");
	}

	namespace pt
	{
		const string NonConcurrencyRuleRemoveAction::PARAMETER_RULE_ID = Action_PARAMETER_PREFIX + "ri";
		
		
		
		ParametersMap NonConcurrencyRuleRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_rule.get()) map.insert(PARAMETER_RULE_ID, _rule->getKey());
			return map;
		}
		
		
		
		void NonConcurrencyRuleRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_rule = NonConcurrencyRuleTableSync::Get(map.get<RegistryKeyType>(PARAMETER_RULE_ID), *_env);
			}
			catch(ObjectNotFoundException<NonConcurrencyRule>& e)
			{
				throw ActionException("Bad rule ID");
			}
		}
		
		
		
		void NonConcurrencyRuleRemoveAction::run(Request& request)
		{
			NonConcurrencyRuleTableSync::Remove(_rule->getKey());
		}
		
		
		
		bool NonConcurrencyRuleRemoveAction::isAuthorized(const Session* session
		) const {
			return _rule.get() && session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_rule->getKey()));
		}



		void NonConcurrencyRuleRemoveAction::setRule( boost::shared_ptr<const env::NonConcurrencyRule> rule )
		{
			_rule = rule;
		}
	}
}
