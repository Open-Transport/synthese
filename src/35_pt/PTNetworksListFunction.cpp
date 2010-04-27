
/** PTNetworksListFunction class implementation.
	@file PTNetworksListFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PTNetworksListFunction.hpp"
#include "Site.h"
#include "Interface.h"
#include "PTNetworkListItemInterfacePage.hpp"
#include "TransportNetworkTableSync.h"
#include "Env.h"
#include "WebPage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<Function,pt::PTNetworksListFunction>::FACTORY_KEY("PTNetworksListFunction");
	
	namespace pt
	{
		const string PTNetworksListFunction::PARAMETER_PAGE_ID("p");
		
		ParametersMap PTNetworksListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}

		void PTNetworksListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id) try
			{
				_page = Env::GetOfficialEnv().get<WebPage>(*id);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such web page");
			}
		}

		void PTNetworksListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			size_t rank(0);
			interfaces::VariablesMap variables;
			if(!_page)
			{
				/// @todo generate XML header
			}
			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(Env::GetOfficialEnv())
			);
			BOOST_FOREACH(const TransportNetworkTableSync::SearchResult::value_type& it, networks)
			{
				if(_page)
				{
					PTNetworkListItemInterfacePage::Display(
						stream,
						_page,
						request,
						*it,
						rank++
					);
				}
				else
				{
					/// @todo generate XML List item
				}
			}
			if(!_page)
			{
				/// @todo generate XML footer
			}
		}
		
		
		
		bool PTNetworksListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTNetworksListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}
	}
}
