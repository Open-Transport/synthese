
/** JourneyBoardInterfacePage class header.
	@file JourneyBoardInterfacePage.h
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

#ifndef SYNTHESE_JourneyBoardInterfacePage_H__
#define SYNTHESE_JourneyBoardInterfacePage_H__

#include "11_interfaces/InterfacePage.h"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	namespace env
	{
		class Journey;
	}

	namespace server
	{
		class Request;
	}

	namespace routeplanner
	{
		/** JourneyBoardInterfacePage Interface Page Class.
			@ingroup m53Pages refPages

			Parameters :
			 - 0 : index of the journey board
			 - 1 : Handicapped filter
			 - 2 : Bike filter
			 - 3 : Departure time
			 - 4 : Departure place name
			 - 5 : Arrival time
			 - 6 : Arrival place name
			 - 7 : Duration
			 - 8 : Departure date
			 - 9 : Reservation possible
			 - 10 : Reservation compulsory
			 - 11 : Reservation delay (minutes)
			 - 12 : Reservation deadline
		*/
		class JourneyBoardInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(
				std::ostream& stream
				, interfaces::VariablesMap& variables
				, int n
				, const env::Journey* journey
				, boost::logic::tribool handicappedFilter
				, boost::logic::tribool bikeFilter
				, const server::Request* request = NULL
			) const;
		};
	}
}

#endif // SYNTHESE_JourneyBoardInterfacePage_H__
