
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEConfigsViewService class implementation.
///	@file InterSYNTHESEConfigsViewService.cpp
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

#include "InterSYNTHESEConfigsViewService.hpp"

#include "InterSYNTHESEConfig.hpp"
#include "InterSYNTHESEConfigItem.hpp"
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
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEConfigsViewService>::FACTORY_KEY = "inter_synthese_configs";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEConfigsViewService::TAG_CONFIG = "config";
		


		ParametersMap InterSYNTHESEConfigsViewService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void InterSYNTHESEConfigsViewService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap InterSYNTHESEConfigsViewService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			BOOST_FOREACH(
				const InterSYNTHESEConfig::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<InterSYNTHESEConfig>()
			){

				boost::shared_ptr<ParametersMap> configPM(new ParametersMap);

				it.second->toParametersMap(*configPM, true);

				map.insert(TAG_CONFIG, configPM);
			}

			return map;
		}
		
		
		
		bool InterSYNTHESEConfigsViewService::isAuthorized(
			const Session* session
		) const {
			return true;
			//session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string InterSYNTHESEConfigsViewService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
