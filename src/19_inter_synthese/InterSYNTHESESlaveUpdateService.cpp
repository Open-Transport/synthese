
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESESlaveUpdateService class implementation.
///	@file InterSYNTHESESlaveUpdateService.cpp
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

#include "InterSYNTHESESlaveUpdateService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServerConstants.h"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESESlaveUpdateService>::FACTORY_KEY = "slave_update";
	
	namespace inter_synthese
	{
		const string InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR = "!$$$$!$$$$!%%%!\r\n";
		const string InterSYNTHESESlaveUpdateService::NO_CONTENT_TO_SYNC = "no_content_to_sync!";
		const string InterSYNTHESESlaveUpdateService::PARAMETER_SLAVE_ID = "slave_id";
		


		ParametersMap InterSYNTHESESlaveUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			if(_slaveId)
			{
				map.insert(PARAMETER_SLAVE_ID, *_slaveId);
			}
			return map;
		}



		void InterSYNTHESESlaveUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_slave = Env::GetOfficialEnv().getEditable<InterSYNTHESESlave>(map.get<RegistryKeyType>(PARAMETER_SLAVE_ID));
			}
			catch (ObjectNotFoundException<InterSYNTHESESlave>&)
			{
				throw RequestException("No such slave");
			}
		}



		ParametersMap InterSYNTHESESlaveUpdateService::run(
			std::ostream& stream,
			const Request& request
		) const {

			InterSYNTHESESlave::QueueRange range(_slave->getQueueRange());
			if(range.first == _slave->getQueue().end())
			{
				stream << NO_CONTENT_TO_SYNC;
			}
			else
			{
				bool first(true);
				for(InterSYNTHESESlave::Queue::iterator it(range.first); it != _slave->getQueue().end(); ++it)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR;
					}
					stream <<
						it->second->get<Key>() << ":" <<
						it->second->get<SyncType>() << ":" <<
						it->second->get<SyncContent>()
					;

					// Exit on last item
					if(it == range.second)
					{
						break;
					}
				}
			}
			_slave->setLastSentRange(range);

			return ParametersMap();
		}
		
		
		
		bool InterSYNTHESESlaveUpdateService::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string InterSYNTHESESlaveUpdateService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
