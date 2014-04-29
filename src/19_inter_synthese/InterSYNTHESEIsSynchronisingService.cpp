
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEIsSynchronisingService class implementation.
///	@file InterSYNTHESEIsSynchronisingService.cpp
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

#include "InterSYNTHESEIsSynchronisingService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServerConstants.h"
#include "ServerModule.h"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEIsSynchronisingService>::FACTORY_KEY = "is_synchronising";
	
	namespace inter_synthese
	{
		int InterSYNTHESEIsSynchronisingService::COMPTEUR = 0;
		const string InterSYNTHESEIsSynchronisingService::TAG_SLAVEQUEUEID = "id_in_each_queue";
		const string InterSYNTHESEIsSynchronisingService::ATTR_COMPTEUR = "compteur";
		const string InterSYNTHESEIsSynchronisingService::ATTR_IS_SYNCHRONISING = "is_synchronising";
		const string InterSYNTHESEIsSynchronisingService::PARAMETER_SLAVE_ID = "slave_id";
		const string InterSYNTHESEIsSynchronisingService::PARAMETER_QUEUEID = "queue_id";
		/*
		bool InterSYNTHESEIsSynchronisingService::bgUpdaterDone(false);
		boost::mutex InterSYNTHESEIsSynchronisingService::bgMutex;
		boost::shared_ptr<InterSYNTHESESlave> InterSYNTHESEIsSynchronisingService::bgNextSlave;*/

		ParametersMap InterSYNTHESEIsSynchronisingService::_getParametersMap() const
		{
//			ParametersMap map(_mapSlvIdQueueId);
			return ParametersMap();
		}



		void InterSYNTHESEIsSynchronisingService::_setFromParametersMap(const ParametersMap& map)
		{
/*			try
			{
				_slave = Env::GetOfficialEnv().getEditable<InterSYNTHESESlave>(map.get<RegistryKeyType>(PARAMETER_SLAVE_ID));
			}
			catch (ObjectNotFoundException<InterSYNTHESESlave>&)
			{
				throw RequestException("No such slave");
			}
*/
		}



		ParametersMap InterSYNTHESEIsSynchronisingService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			if (COMPTEUR >= 3) {
				map.insert(ATTR_IS_SYNCHRONISING, false);
				map.insert(ATTR_COMPTEUR, COMPTEUR);
				COMPTEUR = 0;
			}
			else {
				map.insert(ATTR_IS_SYNCHRONISING, true);
				COMPTEUR++;
				map.insert(ATTR_COMPTEUR, COMPTEUR);
			}

/*			if(_slave->fullUpdateNeeded())
			{
				if(bgProcessSlave(_slave))
				{
					stream << "we are processing your initial dump. come back soon!";
				}
				else
				{
					stream << "sorry another initial dump is running, come back soon";
				}
				return ParametersMap();
			}

			recursive_mutex::scoped_lock queueLock(_slave->getQueueMutex());
			InterSYNTHESESlave::QueueRange range(_slave->getQueueRange());
			if(range.first == _slave->getQueue().end())
			{
				// Send to the slave that there is nothing to sync
				stream << NO_CONTENT_TO_SYNC;
			}
			else
			{
				_slave->sendToSlave(stream, range, _askIdRange);
			}

			// Record the request as slave activity
			// Even if the slave crashed and don't get the results,
			// we won't rebuild the full database on the new slave_update
			_slave->markAsUpToDate();
*/

			return map;
		}
		
		
		
		bool InterSYNTHESEIsSynchronisingService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESEIsSynchronisingService::getOutputMimeType() const
		{
			return "text/html";
		}

}	}
