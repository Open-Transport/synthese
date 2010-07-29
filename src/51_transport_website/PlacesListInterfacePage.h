
/** PlacesListInterfacePage class header.
	@file PlacesListInterfacePage.h
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

#ifndef SYNTHESE_PlacesListInterfacePage_H__
#define SYNTHESE_PlacesListInterfacePage_H__

#include "36_places_list/Types.h"

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace server
	{
		class Request;
	}

	namespace transportwebsite
	{
		class WebPage;

		/** PlacesListInterfacePage Interface Page Class.
			@ingroup m56Pages refPages

			Parameters :
			 - 0 : 1|0 is city list
			 - 1 : 1|0 is for origin selection

			Available Data :
			 - 2 : size of the results
			 - 3 : if places list, city id
			 - 4 : if places list, city name

			Object :
			 - PlacesList list of results
		*/
		class PlacesListInterfacePage
		{
		public:
			static const std::string PARAMETER_IS_CITY_LIST;
			static const std::string PARAMETER_IS_FOR_ORIGIN;

			static const std::string DATA_RESULTS_SIZE;
			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_CONTENT;

			static const std::string DATA_NAME;
			static const std::string DATA_RANK;

		private:
			static void DisplayItem(
				std::ostream& stream,
				boost::shared_ptr<const WebPage> page,
				const server::Request& request,
				std::size_t n,
				const std::string& name,
				util::RegistryKeyType id
			);


		public:
			static void DisplayPlacesList(
				std::ostream& stream,
				boost::shared_ptr<const WebPage> page,
				boost::shared_ptr<const WebPage> itemPage,
				const server::Request& request,
				const PlacesList& results,
				bool isForOrigin,
				const geography::City* city
			);


			static void DisplayCitiesList(
				std::ostream& stream,
				boost::shared_ptr<const WebPage> page,
				boost::shared_ptr<const WebPage> itemPage,
				const server::Request& request,
				const PlacesList& results,
				bool isForOrigin
			);
		};
	}
}

#endif // SYNTHESE_PlacesListInterfacePage_H__
