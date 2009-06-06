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
		class UseRule;
		class AccessParameters;
		
		/////////////////////////////////////////////////////////////////////////
		/// Interface for user of rules defined by UseRules class.
		/// @ingroup m18
		class RuleUser
		{
		public:
			typedef std::map<UserClassCode, const UseRule*> Map;

		private:
			Map _rules;
		
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
				/// use getActualRules() instead.
				const Map& getRules() const;
			//@}
			
			//! @name Setters
			//@{
				void addRule(
					const Map::key_type userClass,
					const Map::mapped_type value
				);
				void remove(
					const Map::key_type userClass
				);
			//@}
			
			//! @name Queries
			//@{
				/////////////////////////////////////////////////////////////////
				/// Consolidated rules query.
				/// @return all the rules defined by the object and its parents.
				Map getActualRules() const;



				//////////////////////////////////////////////////////////////////////////
				/// Use rule fetcher.
				/// Search for a definition of the use rule for the specified user class.
				///  - if found in the object : return it.
				///  - if the object has a parent in the RuleUser tree : recursive call
				///  - it the object has no parent : return the FORBIDDEN default use rule.
				const UseRule&	getUseRule(
					const UserClassCode userClass
				) const;



				bool isCompatibleWith(
					const AccessParameters& accessParameters
				) const;

			//@}
		};
	}
}

#endif //SYNTHESE_RuleUser_h__
