
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEUpdateAckService class implementation.
///	@file InterSYNTHESEUpdateAckService.cpp
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

#include "InterSYNTHESEUpdateAckService.hpp"

#include "InterSYNTHESESlaveTableSync.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEUpdateAckService>::FACTORY_KEY = "inter_synthese_update_ack";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEUpdateAckService::PARAMETER_SLAVE_ID = "slave_id";
		const string InterSYNTHESEUpdateAckService::PARAMETER_RANGE_BEGIN = "range_begin";
		const string InterSYNTHESEUpdateAckService::PARAMETER_RANGE_END = "range_end";
		const string InterSYNTHESEUpdateAckService::VALUE_OK = "OK";
		const string InterSYNTHESEUpdateAckService::VALUE_ERROR = "ERROR";



		ParametersMap InterSYNTHESEUpdateAckService::_getParametersMap() const
		{
			ParametersMap map;
			if(_slaveId)
			{
				map.insert(PARAMETER_SLAVE_ID, *_slaveId);
				map.insert(PARAMETER_RANGE_BEGIN, *_rangeBegin);
				map.insert(PARAMETER_RANGE_END, *_rangeEnd);
			}
			return map;
		}



		void InterSYNTHESEUpdateAckService::_setFromParametersMap(const ParametersMap& map)
		{
			// Slave
			try
			{
				_slave = Env::GetOfficialEnv().getEditable<InterSYNTHESESlave>(
					map.get<RegistryKeyType>(PARAMETER_SLAVE_ID)
				);
				_range = make_pair(
					_slave->getQueue().end(),
					_slave->getQueue().end()
				);
			}
			catch(ObjectNotFoundException<InterSYNTHESESlave>&)
			{
				throw RequestException("No such slave");
			}

			// Range
			if(map.isDefined(PARAMETER_RANGE_BEGIN) && map.isDefined(PARAMETER_RANGE_END))
			{
				_range = make_pair(
					_slave->getQueue().find(map.get<RegistryKeyType>(PARAMETER_RANGE_BEGIN)),
					_slave->getQueue().find(map.get<RegistryKeyType>(PARAMETER_RANGE_END))
				);
			}
		}



		ParametersMap InterSYNTHESEUpdateAckService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if(_range == _slave->getLastSentRange())
			{
				// Confirm the range
				stream << VALUE_OK;
				_slave->clearLastSentRange();

				// Record the request as slave activity
				_slave->markAsUpToDate();
			}
			else
			{
				stream << VALUE_ERROR;
			}

			return map;
		}
		
		
		
		bool InterSYNTHESEUpdateAckService::isAuthorized(
			const Session* session
		) const {
			return true;
			// session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string InterSYNTHESEUpdateAckService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
