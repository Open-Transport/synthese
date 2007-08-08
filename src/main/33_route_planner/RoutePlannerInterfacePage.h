
/** RoutePlannerInterfacePage class header.
	@file RoutePlannerInterfacePage.h

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

#ifndef SYNTHESE_RoutePlannerInterfacePage_H__
#define SYNTHESE_RoutePlannerInterfacePage_H__

#include "33_route_planner/Types.h"

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace time
	{
		class Date;
	}

	namespace server
	{
		class Request;
	}

	namespace env
	{
		class Journey;
		class Place;
	}

	namespace routeplanner
	{
		/** RoutePlannerInterfacePage Interface Page Class.
			@ingroup m33Pages refPages

			Parameters :
				- 0 : date (internal format)
				- 1 : origin city id
				- 2 : origin city text
				- 3 : origin place id
				- 4 : origin place text
				- 5 : destination city id
				- 6 : destination city text
				- 7 : destination place id
				- 8 : destination place text
				- 9 : period
			
		*/
		class RoutePlannerInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const JourneyBoardJourneys& object
				, const time::Date& date
				, int period
				, const env::Place* originPlace
				, const env::Place* destinationPlace
				, const server::Request* request = NULL
				) const;
		};
	}
}

#endif // SYNTHESE_RoutePlannerInterfacePage_H__
