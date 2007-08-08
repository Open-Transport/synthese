
/** FunctionWithSite class header.
	@file FunctionWithSite.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_FunctionWithSite_H__
#define SYNTHESE_FunctionWithSite_H__

#include "30_server/Function.h"

namespace synthese
{
	namespace transportwebsite
	{
		class Site;

		/** FunctionWithSite Function class.
			@author Hugues Romain
			@date 2007
			@ingroup m36
		*/
		class FunctionWithSite : public server::Function
		{
		public:
			static const std::string PARAMETER_SITE;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const Site>	_site;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			virtual void _copy(boost::shared_ptr<const Function> function);

		public:

			boost::shared_ptr<const Site> getSite() const;
			void setSite(boost::shared_ptr<const Site>);

		};
	}
}

#endif // SYNTHESE_FunctionWithSite_H__
