
/** AlgorithmModule class header.
	@file AlgorithmModule.h
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

#ifndef SYNTHESE_RoutePlannerModule_H__
#define SYNTHESE_RoutePlannerModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/** 33 Algorithm module namespace.
	@author Hugues Romain
	@date 2007
	*/
	namespace algorithm
	{
		/**	@defgroup m33Actions 33.15 Actions
			@ingroup m33

			@defgroup m33Pages 33.31 Pages
			@ingroup m33

			@defgroup m33Functions 33.15 Functions
			@ingroup m33

			@defgroup m33Admin 33 Administration pages
			@ingroup m33

			@defgroup m33Rights 33 Rights
			@ingroup m33

			@defgroup m33Logs 33 DB Logs
			@ingroup m33

			@defgroup m33 33 Algorithm
			@ingroup m3

			@{
		*/

		/** 33 Algorithm Module class.
			@author Hugues Romain
			@date 2007
		*/
		class AlgorithmModule:
			public server::ModuleClassTemplate<AlgorithmModule>
		{
		public:
			static const std::string MODULE_PARAM_USE_ASTAR_FOR_PHYSICAL_STOPS_EXTENDER;

		private:
			static bool _useAStarForPhysicalStopsExtender;

		public:
			static bool GetUseAStarForPhysicalStopsExtender(){ return _useAStarForPhysicalStopsExtender; }



			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_RoutePlannerModule_H__
