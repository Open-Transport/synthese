
//////////////////////////////////////////////////////////////////////////////////////////
///	OperationUnitsService class implementation.
///	@file OperationUnitsService.cpp
///	@author hromain
///	@date 2013
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

#include "OperationUnitsService.hpp"

#include "CommercialLine.h"
#include "OperationUnit.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,pt_operation::OperationUnitsService>::FACTORY_KEY = "operation_units";
	
	namespace pt_operation
	{
		const string OperationUnitsService::TAG_OPERATION_UNIT = "operation_unit";
		


		ParametersMap OperationUnitsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void OperationUnitsService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap OperationUnitsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			// Export the operation units
			BOOST_FOREACH(const OperationUnit::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<OperationUnit>())
			{
				boost::shared_ptr<ParametersMap> ouPM(new ParametersMap);
				it.second->toParametersMap(*ouPM, true);
				map.insert(TAG_OPERATION_UNIT, ouPM);
			}

			return map;
		}
		
		
		
		bool OperationUnitsService::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string OperationUnitsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}