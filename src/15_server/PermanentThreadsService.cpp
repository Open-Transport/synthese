
//////////////////////////////////////////////////////////////////////////////////////////
///	PermanentThreadsService class implementation.
///	@file PermanentThreadsService.cpp
///	@author Camille Hue
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

#include "PermanentThreadsService.hpp"

#include "PermanentThread.hpp"
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
	const string FactorableTemplate<Function,server::PermanentThreadsService>::FACTORY_KEY = "permanent_threads";
	
	namespace server
	{
		const string PermanentThreadsService::TAG_PERMANENT_THREAD = "permanent_thread";
		


		ParametersMap PermanentThreadsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void PermanentThreadsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Permanent thread filter
			RegistryKeyType permanentThreadId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(permanentThreadId) try
			{
				_permanentThreadFilter = Env::GetOfficialEnv().get<PermanentThread>(
					permanentThreadId
				);
			}
			catch(ObjectNotFoundException<PermanentThread>&)
			{
				throw RequestException("No such permanent thread");
			}
		}



		ParametersMap PermanentThreadsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			if(_permanentThreadFilter.get())
			{
				boost::shared_ptr<ParametersMap> permanentThreadPM(new ParametersMap);
				_permanentThreadFilter->toParametersMap(*permanentThreadPM, true);
				map.insert(TAG_PERMANENT_THREAD, permanentThreadPM);
			}
			else
			{
				BOOST_FOREACH(const Registry<PermanentThread>::value_type& it, Env::GetOfficialEnv().getRegistry<PermanentThread>())
				{
					boost::shared_ptr<ParametersMap> permanentThreadPM(new ParametersMap);
					it.second->toParametersMap(*permanentThreadPM, true);
					map.insert(TAG_PERMANENT_THREAD, permanentThreadPM);
				}
			}
		
			return map;
		}
		
		
		
		bool PermanentThreadsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PermanentThreadsService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
