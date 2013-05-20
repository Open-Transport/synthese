
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESESlavesViewService class implementation.
///	@file InterSYNTHESESlavesViewService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "InterSYNTHESESlavesViewService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESlave.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESESlavesViewService>::FACTORY_KEY = "inter_synthese_slaves_view";
	
	namespace inter_synthese
	{
		const string InterSYNTHESESlavesViewService::TAG_SLAVE = "slave";



		ParametersMap InterSYNTHESESlavesViewService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void InterSYNTHESESlavesViewService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap InterSYNTHESESlavesViewService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			BOOST_FOREACH(
				const InterSYNTHESESlave::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<InterSYNTHESESlave>()
			){

				boost::shared_ptr<ParametersMap> slavePM(new ParametersMap);

				it.second->toParametersMap(*slavePM, true);

				map.insert(TAG_SLAVE, slavePM);
			}

			return map;
		}
		
		
		
		bool InterSYNTHESESlavesViewService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESESlavesViewService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
