
/** FunctionWithSiteBase class header.
	@file FunctionWithSiteBase.hpp
	@author Hugues Romain
	@date 2007

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_FunctionWithSiteBase_H__
#define SYNTHESE_FunctionWithSiteBase_H__

#include "Function.h"
#include "Webpage.h"

namespace synthese
{
	namespace cms
	{
		/** Function With website class.
			@author Hugues Romain
			@date 2007
			@ingroup m36
		*/
		class FunctionWithSiteBase:
			public server::Function
		{
		public:
			static const std::string PARAMETER_SITE;

		protected:
			//! \name Page parameters
			//@{
				const Website*	_site;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			//////////////////////////////////////////////////////////////////////////
			/// Public function with site copy.
			/// @param function function to copy
			/// @author Hugues Romain
			virtual void _copy(const server::Function& function);

		public:
			//! @name Getters
			//@{
				const Website* getSite() const {	return _site; }
			//@}

			//! @name Setters
			//@{
				void setSite(const Website* value) {	_site = value; }
			//@}

			FunctionWithSiteBase();

			const Webpage* getPage(const std::string& idOrSmartURL) const;
		};
	}
}

#endif // SYNTHESE_FunctionWithSite_H__
