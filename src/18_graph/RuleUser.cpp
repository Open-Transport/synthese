//
// C++ Implementation: RuleUser
//
// Description: 
//
//
// Author: Hugues Romain (RCS) <hugues.romain@reseaux-conseil.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "RuleUser.h"
#include "UseRules.h"

namespace synthese
{
	namespace graph
	{
		RuleUser::RuleUser(
		):	_rules(NULL)
		{
		}
		
		
		const UseRules* RuleUser::getRules() const
		{
			return _rules;
		}
		
		
		
		const UseRules* RuleUser::getActualRules() const
		{
			if (_rules != NULL) return _rules;
			if (_getParentRuleUser() != NULL) return _getParentRuleUser()->getActualRules();
			return NULL;
		}
		
		
		util::RegistryKeyType RuleUser::getActualRulesId() const
		{
			const UseRules* rules(getActualRules());
			return
				(rules == NULL) ?
				util::RegistryKeyType(0) :
				rules->getKey();
		}
		
		
		const UseRule&	RuleUser::getUseRule(UserClassCode userClass) const
		{
			const UseRules* rules(getActualRules());
			return
				(rules == NULL) ?
				UseRule::ALLOWED :
				rules->getUseRule(userClass);
		}
		
		
		
		void RuleUser::setRules(const UseRules* value)
		{
			_rules = value;
		}
	}
}