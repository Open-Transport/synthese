
/** RoutePlannerFunction class header.
	@file RoutePlannerFunction.h

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

#ifndef SYNTHESE_RoutePlannerFunction_H__
#define SYNTHESE_RoutePlannerFunction_H__

#include "30_server/Function.h"

namespace synthese
{
	namespace routeplanner
	{
		/** RoutePlannerFunction class.
			@ingroup m33
		*/
		class RoutePlannerFunction : public server::Function
		{
			static const std::string PARAMETER_SITE;
			
			//! \name Parameters
			//@{
				Site*			_site;
				InterfacePage*	_page;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			RoutePlannerFunction();
			~RoutePlannerFunction();

			/// @todo Getters/Setters for parsed parameters

			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream);
		};
	}
}
#endif // SYNTHESE_RoutePlannerFunction_H__
