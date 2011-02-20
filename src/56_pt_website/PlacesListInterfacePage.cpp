
/** PlacesListInterfacePage class implementation.
	@file PlacesListInterfacePage.cpp

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

#include "PlacesListInterfacePage.h"
#include "City.h"
#include "Request.h"
#include "Function.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace geography;
	using namespace server;
	using namespace cms;

	namespace pt_website
	{
		const std::string PlacesListInterfacePage::PARAMETER_IS_CITY_LIST("is_city_list");
		const std::string PlacesListInterfacePage::PARAMETER_IS_FOR_ORIGIN("is_for_origin");

		const std::string PlacesListInterfacePage::DATA_CITY_ID("city_id");
		const std::string PlacesListInterfacePage::DATA_CITY_NAME("city_name");
		const std::string PlacesListInterfacePage::DATA_RESULTS_SIZE("size");
		const std::string PlacesListInterfacePage::DATA_CONTENT("content");

		const string PlacesListInterfacePage::DATA_NAME("name");
		const string PlacesListInterfacePage::DATA_RANK("rank");

		void PlacesListInterfacePage::DisplayPlacesList(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			boost::shared_ptr<const Webpage> itemPage,
			const server::Request& request,
			const PlacesList& results,
			bool isForOrigin,
			const geography::City* city
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(PARAMETER_IS_CITY_LIST, false);
			pm.insert(PARAMETER_IS_FOR_ORIGIN, isForOrigin);
			pm.insert(DATA_RESULTS_SIZE, results.size());
			if(city)
			{
				pm.insert(DATA_CITY_ID, city->getKey());
				pm.insert(DATA_CITY_NAME, city->getName());
			}

			if(itemPage.get())
			{
				stringstream content;
				size_t i(0);
				for (PlacesList::const_iterator it(results.begin()); it != results.end(); ++it, ++i)
				{
					DisplayItem(
						content,
						itemPage,
						request,
						i,
						it->second,
						it->first
					);
				}
				pm.insert(DATA_CONTENT, content.str());
			}

			page->display(stream, request, pm);
		}



		void PlacesListInterfacePage::DisplayCitiesList(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			boost::shared_ptr<const Webpage> itemPage,
			const server::Request& request,
			const PlacesList& results,
			bool isForOrigin
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(PARAMETER_IS_CITY_LIST, true);
			pm.insert(PARAMETER_IS_FOR_ORIGIN, isForOrigin);
			pm.insert(DATA_RESULTS_SIZE, results.size());

			if(itemPage.get())
			{
				stringstream content;
				size_t i(0);
				for (PlacesList::const_iterator it(results.begin()); it != results.end(); ++it, ++i)
				{
					DisplayItem(
						content,
						itemPage,
						request,
						i,
						it->second,
						it->first
					);
				}
				pm.insert(DATA_CONTENT, content.str());
			}

			page->display(stream, request, pm);
		}



		void PlacesListInterfacePage::DisplayItem(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			const server::Request& request,
			std::size_t n,
			const std::string& name,
			util::RegistryKeyType id
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_RANK, n);
			pm.insert(DATA_NAME, name);
			pm.insert(Request::PARAMETER_OBJECT_ID, id);

			page->display(stream, request, pm);
		}
}	}
