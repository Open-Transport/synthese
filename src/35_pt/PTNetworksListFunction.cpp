
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
#include "TransportNetworkTableSync.h"
#include "Env.h"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt::PTNetworksListFunction>::FACTORY_KEY("PTNetworksListFunction");

	namespace pt
	{
		const string PTNetworksListFunction::PARAMETER_PAGE_ID("p");

		const string PTNetworksListFunction::DATA_NETWORK_ID("network_id");
		const string PTNetworksListFunction::DATA_NAME("name");
		const string PTNetworksListFunction::DATA_RANK("rank");
		const string PTNetworksListFunction::DATA_RANK_IS_ODD("rank_is_odd");

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
				_page = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}
		}

		void PTNetworksListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			size_t rank(0);
			if(!_page.get())
			{
				/// @todo generate XML header
			}
			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(Env::GetOfficialEnv())
			);
			BOOST_FOREACH(const TransportNetworkTableSync::SearchResult::value_type& it, networks)
			{
				if(_page.get())
				{
					_displayNetwork(
						stream,
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
			if(!_page.get())
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



		void PTNetworksListFunction::_displayNetwork(
			std::ostream& stream,
			const server::Request& request,
			const pt::TransportNetwork& object,
			std::size_t rank
		) const {

			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_NETWORK_ID, object.getKey());
			pm.insert(DATA_NAME, object.getName()); //1
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 2); //3

			_page->display(stream, request, pm);
		}
	}
}
