
/** PTNetworkListItemInterfacePage class implementation.
	@file PTNetworkListItemInterfacePage.cpp
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

#include "PTNetworkListItemInterfacePage.hpp"
#include "TransportNetwork.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace pt;
	using namespace cms;
	using namespace server;

	namespace pt
	{
		const string PTNetworkListItemInterfacePage::DATA_NAME("name");	
		const string PTNetworkListItemInterfacePage::DATA_RANK("rank");
		const string PTNetworkListItemInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");
		

		void PTNetworkListItemInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			const server::Request& request,
			const pt::TransportNetwork& object,
			std::size_t rank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;
		
			pm.insert(Request::PARAMETER_OBJECT_ID, object.getKey());
			pm.insert(DATA_NAME, object.getName()); //1
			pm.insert(DATA_RANK, rank);
			pm.insert(DATA_RANK_IS_ODD, rank % 2); //3

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}
