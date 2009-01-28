//
// C++ Implementation: UseRules
//
// Description: 
//
//
// Author: Hugues Romain (RCS) <hugues.romain@reseaux-conseil.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "UseRules.h"

#include <boost/foreach.hpp>

namespace synthese
{
	namespace graph
	{
		const std::string& UseRules::getName() const
		{
			return _name;
		}
		
		const UseRule& UseRules::getUseRule(
			UserClassCode userClass
		) const {
			// The access is defined specifically for the designated user class
			Map::const_iterator it(_useRules.find(userClass));
			if (it != _useRules.end()) return it->second;
			
			// The access is explicitely defined for an othe user class
			BOOST_FOREACH(Map::value_type rule, _useRules)
			{
				if(rule.second.getAccess() == UseRule::ACCESS_COMPULSORY) return UseRule::FORBIDDEN;
			}
			
			// The default access is explicit
			if (_defaultUseRule.getAccess() == UseRule::ACCESS_COMPULSORY ||
				_defaultUseRule.getAccess() == UseRule::ACCESS_FORBIDDEN
			){
				return _defaultUseRule;
			}
			
			// Unknown access is now returned
			return UseRule::UNKNOWN;
		}
	}
}