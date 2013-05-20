
/** HikingTrailSearchFunction class implementation.
	@file HikingTrailSearchFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "HikingTrailSearchFunction.h"
#include "HikingTrailTableSync.h"
#include "ParametersMap.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,hiking::HikingTrailSearchFunction>::FACTORY_KEY("HikingTrailSearchFunction");

	namespace hiking
	{
		const string HikingTrailSearchFunction::PARAMETER_SEARCH_NAME = "sn";
		const string HikingTrailSearchFunction::PARAMETER_ITEM_DISPLAY_PAGE_ID = "pi";

		const string HikingTrailSearchFunction::DATA_HIKING_TRAIL = "hiking_trail";

		ParametersMap HikingTrailSearchFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_searchName)
			{
				map.insert(PARAMETER_SEARCH_NAME, *_searchName);
			}
			return map;
		}

		void HikingTrailSearchFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_searchName = map.getOptional<string>(PARAMETER_SEARCH_NAME);

			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_ITEM_DISPLAY_PAGE_ID));
			if(id) try
			{
				_itemDisplayPage = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}
		}



		ParametersMap HikingTrailSearchFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			HikingTrailTableSync::SearchResult trails(
				HikingTrailTableSync::Search(Env::GetOfficialEnv(), _searchName)
			);
			BOOST_FOREACH(
				HikingTrailTableSync::SearchResult::value_type& trail,
				trails
			){
				boost::shared_ptr<ParametersMap> itemMap(new ParametersMap);

				itemMap->insert(Request::PARAMETER_OBJECT_ID, trail->getKey());

				pm.insert(DATA_HIKING_TRAIL, itemMap);
			}

			if(_itemDisplayPage.get())
			{
				BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& itemMap, pm.getSubMaps(DATA_HIKING_TRAIL))
				{
					itemMap->merge(getTemplateParameters());

					_itemDisplayPage->display(stream, request, *itemMap);
				}
			}

			return pm;
		}



		bool HikingTrailSearchFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string HikingTrailSearchFunction::getOutputMimeType() const
		{
			return _itemDisplayPage.get() ? _itemDisplayPage->getMimeType() : "text/xml";
		}
}	}
