
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

#include "36_places_list/Types.h"

#include "33_route_planner/Types.h"

#include "11_interfaces/InterfacePage.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace transportwebsite
	{
		class HourPeriod;
		class Site;
	}

	namespace time
	{
		class Date;
	}

	namespace server
	{
		class Request;
	}

	namespace geography
	{
		class Place;
	}

	namespace graph
	{
		class Journey;
		class AccessParameters;
	}

	namespace routeplanner
	{
		/** RoutePlannerInterfacePage Interface Page Class.
			@ingroup m53Pages refPages

			Parameters :
				- 0 : date (internal format)
				- 1 : home display
				- 2 : origin city text
				- 3 : handicapped filter
				- 4 : origin place text
				- 5 : bike filter
				- 6 : destination city text
				- 7 : destination place id
				- 8 : destination place text
				- 9 : period (id)
				- 10 : date (text format)
				- 11 : period (text format)
				- 12 : solutions number
				- 13 : accessibility code
				- 14 : site id
				- 15 : empty result because of same places
		*/
		class RoutePlannerInterfacePage : public util::FactorableTemplate<interfaces::InterfacePage,RoutePlannerInterfacePage>
		{
		public:
			RoutePlannerInterfacePage();

			/** Display of a successful route planning
				Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const RoutePlannerResult& object
				, const time::Date& date
				, int periodId
				, const geography::Place* originPlace
				, const geography::Place* destinationPlace
				, const transportwebsite::HourPeriod* period
				, const graph::AccessParameters& accessParameters
				, const server::Request* request
				, const transportwebsite::AccessibilityParameter& accessibility
				, const transportwebsite::Site* site
				, bool samePlaces
			) const;

			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, const time::Date& date
				, int periodId
				, bool home
				, const std::string& originCity
				, const std::string& originPlace
				, const std::string& destinationCity
				, const std::string& destinationPlace
				, const transportwebsite::HourPeriod* period
				, const graph::AccessParameters& accessParameters
				, const server::Request* request
				, const transportwebsite::AccessibilityParameter& accessibility
				, const transportwebsite::Site* site
			) const;
		};
	}
}

#endif // SYNTHESE_RoutePlannerInterfacePage_H__
