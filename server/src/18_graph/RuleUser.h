////////////////////////////////////////////////////////////////////////////////
/// RuleUser class header.
///	@file RuleUser.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "GraphConstants.h"
#include "UseRule.h"

#include <vector>
#include <boost/utility.hpp>

namespace synthese
{
	namespace graph
	{
		class UseRule;
		class AccessParameters;

		/////////////////////////////////////////////////////////////////////////
		/// Interface for user of rules defined by UseRules class.
		/// @ingroup m18
		/// @author Hugues Romain
		class RuleUser : boost::noncopyable
		{
		public:
			typedef std::vector<const UseRule*> Rules;

		private:
			Rules _rules;

		protected:

			/////////////////////////////////////////////////////////////////////
			/// Constructor.
			RuleUser() :
			_rules(USER_CLASSES_VECTOR_SIZE, NULL)
			{}

		public:
			//! @name Getters
			//@{
				/////////////////////////////////////////////////////////////////
				/// Rules getter.
				/// @return only the rules registered to the current object.
				/// To obtain the actual rule inherited from parent object,
				/// use getActualRules() instead.
				const Rules& getRules() const { return _rules; }
			//@}

			//! @name Setters
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Rules setter.
				/// @param value rules
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				virtual void setRules(const Rules& value){ _rules = value; }
			//@}

			//! @name Queries
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the parent object to read if the current object does
				/// not define the needed rule.
				/// @return the parent object
				virtual const RuleUser* _getParentRuleUser() const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Use rule fetcher.
				/// Search for a definition of the use rule for the specified user class.
				///  - if found in the object : return it.
				///  - if the object has a parent in the RuleUser tree : recursive call
				///  - it the object has no parent : return the FORBIDDEN default use rule.
				/// @param rank rank of user class. To deduce the rank from the user class code,
				///		do rank = classCode - USER_CLASS_CODE_OFFSET
				/// @return the rule to apply to the user
				/// @warning the value of rank is not checked. It must be lower than USER_CLASSES_VECTOR_SIZE.
				const UseRule&	getUseRule(
					std::size_t rank
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the use rule recorded to the object (without recursion).
				/// @param userClassCode class code (will be translated into rank)
				/// @return pointer to the recorder rule or NULL if non defined
				const UseRule* getRule(
					UserClassCode classCode
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the rule user is compatible with access parameters.
				/// @param accessParameters access parameters
				/// @return true if the rule user is compatible with the specified parameters
				/// @author Hugues Romain
				bool isCompatibleWith(
					const AccessParameters& accessParameters
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Pure virtual name getter.
				/// @return the name of the object
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual std::string getRuleUserName() const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Generates an empty vector of rules.
				/// @return vector of rules initialized to NULL
				/// @date 2010
				/// @since 3.1.18
				/// @author Hugues Romain
				static Rules GetEmptyRules();
			//@}
		};
	}
}

#endif //SYNTHESE_RuleUser_h__
