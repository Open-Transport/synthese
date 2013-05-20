
/** GetDepartureBoardsService class implementation.
	@file GetDepartureBoardsService.cpp
	@author Hugues Romain
	@date 2011

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
#include "GetDepartureBoardsService.hpp"
#include "StopArea.hpp"
#include "Webpage.h"
#include "DisplayScreenTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,departure_boards::GetDepartureBoardsService>::FACTORY_KEY("GetDepartureBoards");

	namespace departure_boards
	{
		const string GetDepartureBoardsService::PARAMETER_STOP_ID("stop_id");
		const string GetDepartureBoardsService::PARAMETER_PAGE_ID("page_id");

		const string GetDepartureBoardsService::DATA_SCREENS("screens");
		const string GetDepartureBoardsService::DATA_SCREEN("screen");
		const string GetDepartureBoardsService::DATA_RANK("rank");

		ParametersMap GetDepartureBoardsService::_getParametersMap() const
		{
			ParametersMap map;
			if(_stopArea.get())
			{
				map.insert(PARAMETER_STOP_ID, _stopArea->getKey());
			}
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}

		void GetDepartureBoardsService::_setFromParametersMap(const ParametersMap& map)
		{
			{ // Optional stop filter
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_STOP_ID, 0));
				if(id) try
				{
					_stopArea = Env::GetOfficialEnv().get<StopArea>(id);
				}
				catch (ObjectNotFoundException<StopArea>&)
				{
					throw "No such stop area";
				}
			}

			{ // CMS page
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_PAGE_ID, 0));
				if(id) try
				{
					_page = Env::GetOfficialEnv().get<Webpage>(id);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such page");
				}
			}

			if(!_page.get())
			{
				setOutputFormatFromMap(map, string());
			}
		}



		util::ParametersMap GetDepartureBoardsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			// Loop on screens
			DisplayScreenTableSync::SearchResult screens(
				DisplayScreenTableSync::Search(
					Env::GetOfficialEnv(),
					optional<const RightsOfSameClassMap&>(),
					true,
					FORBIDDEN,
					optional<RegistryKeyType>(),
					_stopArea.get() ? optional<RegistryKeyType>(_stopArea->getKey()) : optional<RegistryKeyType>()
			)	);
			BOOST_FOREACH(const boost::shared_ptr<DisplayScreen>& screen, screens)
			{
				// Declaration
				boost::shared_ptr<ParametersMap> pmScreen(new ParametersMap);

				screen->toParametersMap(*pmScreen);

				pm.insert(DATA_SCREEN, pmScreen);
			}

			if(_page.get()) // CMS output
			{
				size_t rank(0);
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& pmScreen, pm.getSubMaps(DATA_SCREEN))
				{
					// Template parameters
					pmScreen->merge(getTemplateParameters());

					// Rank
					pmScreen->insert(DATA_RANK, rank++);

					// Display
					_page->display(stream, request, *pmScreen);
				}
			}
			else if(_outputFormat == MimeTypes::XML)
			{
				pm.outputXML(
					stream,
					DATA_SCREENS,
					true,
					"http://synthese.rcsmobility.com/include/54_departure_boards/GetDepartureBoards.xsd"
				);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				pm.outputJSON(
					stream,
					DATA_SCREENS
				);
			}

			return pm;
		}



		bool GetDepartureBoardsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetDepartureBoardsService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}
	}
}
