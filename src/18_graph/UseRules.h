////////////////////////////////////////////////////////////////////////////////
/// UseRules class header.
///	@file UseRules.h
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

#ifndef SYNTHESE_UseRules_h__
#define SYNTHESE_UseRules_h__

#include "Registrable.h"
#include "Registry.h"
#include "UseRule.h"
#include "GraphTypes.h"

#include <string>
#include <map>

namespace synthese
{
	namespace graph
	{
		///////////////////////////
		/// Use rules class.
		/// @ingroup m18
		class UseRules
		:	public virtual util::Registrable
		{
		public:

		private:
			typedef std::map<UserClassCode, UseRule> Map;
		
			std::string		_name;
			UseRule			_defaultUseRule;
			Map				_useRules;
			
		protected:
			UseRules(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);
			
		public:
			//! @name Getters
			//@{
				const std::string&	getName() const;
			//@}
			
			//! @name Queries
			//@{
				/////////////////////////////////////////////////////////////////
				/// Use rule getter.
				/// @param userClass class of the user, which is used to determinate
				///		the applicable rule
				const UseRule&	getUseRule(UserClassCode userClass) const;
			//@}
			
			//! @name Setters
			//@{
				void setName(const std::string& value);
				void addRule(
					UserClassCode userClass,
					const UseRule& rule
				);
				void setDefaultUseRule(
					const UseRule& rule
				);
			//@}
		};
	}
}

#endif