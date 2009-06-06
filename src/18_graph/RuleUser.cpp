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
#include "ForbiddenUseRule.h"
#include "Exception.h"
#include "AccessParameters.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	
	namespace graph
	{
		RuleUser::RuleUser(
		){
		}
		
		
		const RuleUser::Map& RuleUser::getRules() const
		{
			return _rules;
		}
		
		
		
		RuleUser::Map RuleUser::getActualRules() const
		{
			Map result;
			for(const RuleUser* ruleUser(this); ruleUser != NULL; ruleUser = ruleUser->_getParentRuleUser())
			{
				BOOST_FOREACH(const Map::value_type& it, _rules)
				{
					Map::const_iterator its(result.find(it.first));
					if(its != result.end())
					{
						continue;
					}
					result.insert(make_pair(it.first, it.second));
				}
			}
			return result;
		}
		
		

		const UseRule& RuleUser::getUseRule(
			const UserClassCode userClass
		) const	{
			Map::const_iterator it(_rules.find(userClass));
			if(it != _rules.end())
			{
				return *it->second;
			}
			if (_getParentRuleUser() == NULL)
			{
				return *ForbiddenUseRule::INSTANCE;
			}
			return _getParentRuleUser()->getUseRule(userClass);
		}
		
		
		
		void RuleUser::addRule(
			const RuleUser::Map::key_type userClass,
			const RuleUser::Map::mapped_type value
		){
			_rules[userClass] = value;
		}



		void RuleUser::remove(
			const RuleUser::Map::key_type userClass
		){
			Map::iterator it(_rules.find(userClass));
			if(it != _rules.end())
			{
				_rules.erase(it);
			}
		}

		bool RuleUser::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			const UseRule& rule(getUseRule(accessParameters.getUserClass()));
			return rule.isCompatibleWith(accessParameters);
		}

		const RuleUser::Map::mapped_type RuleUser::getRule(
			const Map::key_type userClass
		) const	{
			Map::const_iterator it(_rules.find(userClass));
			if(it == _rules.end()) return NULL;
			return it->second;
		}
	}
}