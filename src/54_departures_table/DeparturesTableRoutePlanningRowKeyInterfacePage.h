
/** DeparturesTableRoutePlanningRowKeyInterfacePage class header.
	@file DeparturesTableRoutePlanningRowKeyInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_DeparturesTableRoutePlanningRowKeyInterfacePage_H__
#define SYNTHESE_DeparturesTableRoutePlanningRowKeyInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		/** DeparturesTableRoutePlanningRowKeyInterfacePage Interface Page Class.
			@ingroup m54Pages refPages
			@author Hugues
			@date 2009

			@code departures_table_route_planning_row_key @endcode

			Parameters :
				- 17 : City name
				- 18 : City is different than display screen place
				- 19 : Place name
				- 20 : Place name (13 chars)
				- 21 : Place name (26 chars)
		*/
		class DeparturesTableRoutePlanningRowKeyInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, DeparturesTableRoutePlanningRowKeyInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...	
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const env::PublicTransportStopZoneConnectionPlace& origin,
				const RoutePlanningRow& row ,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;
			
			DeparturesTableRoutePlanningRowKeyInterfacePage();
		};
	}
}

#endif // SYNTHESE_DeparturesTableRoutePlanningRowKeyInterfacePage_H__
