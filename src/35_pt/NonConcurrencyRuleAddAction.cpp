
/** NonConcurrencyRuleAddAction class implementation.
	@file NonConcurrencyRuleAddAction.cpp
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
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "NonConcurrencyRuleAddAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::NonConcurrencyRuleAddAction>::FACTORY_KEY("NonConcurrencyRuleAddAction");
	}

	namespace pt
	{
		const string NonConcurrencyRuleAddAction::PARAMETER_PRIORITY_LINE_ID = Action_PARAMETER_PREFIX + "pl";
		const string NonConcurrencyRuleAddAction::PARAMETER_HIDDEN_LINE_ID = Action_PARAMETER_PREFIX + "hl";
		const string NonConcurrencyRuleAddAction::PARAMETER_DURATION = Action_PARAMETER_PREFIX + "de";



		ParametersMap NonConcurrencyRuleAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_priorityLine.get()) map.insert(PARAMETER_PRIORITY_LINE_ID, _priorityLine->getKey());
			if(_hiddenLine.get()) map.insert(PARAMETER_HIDDEN_LINE_ID, _hiddenLine->getKey());
			map.insert(PARAMETER_DURATION, _duration.minutes());
			return map;
		}



		void NonConcurrencyRuleAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_priorityLine = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PRIORITY_LINE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("Bad priority line");
			}
			try
			{
				_hiddenLine = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_HIDDEN_LINE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("Bad hidden line");
			}
			_duration = posix_time::minutes(map.get<int>(PARAMETER_DURATION));
		}



		void NonConcurrencyRuleAddAction::run(Request& request)
		{
			NonConcurrencyRule r;
			r.setPriorityLine(_priorityLine.get());
			r.setHiddenLine(_hiddenLine.get());
			r.setDelay(_duration);

			NonConcurrencyRuleTableSync::Save(&r);
		}



		bool NonConcurrencyRuleAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}



		void NonConcurrencyRuleAddAction::setPriorityLine( boost::shared_ptr<const CommercialLine> value )
		{
			_priorityLine = const_pointer_cast<CommercialLine>(value);
		}



		void NonConcurrencyRuleAddAction::setHiddenLine( boost::shared_ptr<const CommercialLine> value )
		{
			_hiddenLine = const_pointer_cast<CommercialLine>(value);
		}
	}
}
