////////////////////////////////////////////////////////////////////////////////
/// RuleUser class header.
///	@file RuleUser.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_RuleUser_h__
#define SYNTHESE_RuleUser_h__

#include "Registry.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace graph
	{
		class UseRules;
		class UseRule;
		
		/////////////////////////////////////////////////////////////////////////
		/// Interface for user of rules defined by UseRules class.
		/// @ingroup m18
		class RuleUser
		{
		private:
			const UseRules* _rules;
		
		protected:
			virtual const RuleUser* _getParentRuleUser() const = 0;
			
			/////////////////////////////////////////////////////////////////////
			/// Constructor.
			RuleUser();
			
		public:
			//! @name Getters
			//@{
				/////////////////////////////////////////////////////////////////
				/// Rules getter.
				/// @return only the rules registered to the current object.
				/// To obtain the actual rule inherited from parent object,
				/// use getUseRule() instead.
				const UseRules* getRules() const;
			//@}
			
			//! @name Setters
			//@{
				void setRules(const UseRules* value);
			//@}
			
			//! @name Queries
			//@{
				const UseRules* getActualRules() const;
				const UseRule&	getUseRule(UserClassCode userClass) const;
				util::RegistryKeyType getActualRulesId() const;
			//@}
		};
	}
}

#endif //SYNTHESE_RuleUser_h__
