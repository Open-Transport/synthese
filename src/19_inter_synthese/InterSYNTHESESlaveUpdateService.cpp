
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
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESESlaveUpdateService>::FACTORY_KEY = "slave_update";
	
	namespace inter_synthese
	{
		const string InterSYNTHESESlaveUpdateService::NO_CONTENT_TO_SYNC = "no_content_to_sync!";
		const string InterSYNTHESESlaveUpdateService::PARAMETER_SLAVE_ID = "slave_id";
		const string InterSYNTHESESlaveUpdateService::PARAMETER_ASK_ID_RANGE = "ask_id_range";
		const string InterSYNTHESESlaveUpdateService::PARAMETER_STARTING_TIME = "starting_date";
		
		bool InterSYNTHESESlaveUpdateService::bgUpdaterDone(false);
		boost::mutex InterSYNTHESESlaveUpdateService::bgMutex;
		boost::shared_ptr<InterSYNTHESESlave> InterSYNTHESESlaveUpdateService::bgNextSlave;
		boost::mutex InterSYNTHESESlaveUpdateService::bgRTMutex;
		boost::shared_ptr<InterSYNTHESESlave> InterSYNTHESESlaveUpdateService::bgRTNextSlave;

		ParametersMap InterSYNTHESESlaveUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			if(_slaveId)
			{
				map.insert(PARAMETER_SLAVE_ID, *_slaveId);
			}

			map.insert(PARAMETER_ASK_ID_RANGE, _askIdRange);
			
			if (!_slaveStartingDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_STARTING_TIME, _slaveStartingDate);
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

			_askIdRange = map.getDefault<bool>(PARAMETER_ASK_ID_RANGE, false);
			
			_slaveStartingDate = not_a_date_time;
			if (map.isDefined(PARAMETER_STARTING_TIME))
			{
				_slaveStartingDate = time_from_string(map.get<string>(PARAMETER_STARTING_TIME));
			}
		}



		ParametersMap InterSYNTHESESlaveUpdateService::run(
			std::ostream& stream,
			const Request& request
		) const {

			if(_slave->fullUpdateNeeded())
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
			
			// Full RT dump if LastActivityreport < SlaveStartingDate
			util::Log::GetInstance().debug("InterSYNTHESESlaveUpdateService : test RT full dump");
			if (!_slaveStartingDate.is_not_a_date_time() &&
				_slaveStartingDate > _slave->get<LastActivityReport>())
			{
				if(bgRTProcessSlave(_slave))
				{
					stream << "we are processing your initial Real Time dump. come back soon!";
					util::Log::GetInstance().debug("InterSYNTHESESlaveUpdateService : we are processing your initial Real Time dump. come back soon!");
				}
				else
				{
					stream << "sorry another initial Real Time dump is running, come back soon";
				}
				return ParametersMap();
			}
			util::Log::GetInstance().debug("InterSYNTHESESlaveUpdateService : after test RT full dump");

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

		bool InterSYNTHESESlaveUpdateService::bgProcessSlave(
			const boost::shared_ptr<InterSYNTHESESlave> &slave
		) const
		{
			boost::mutex::scoped_lock(bgMutex);
			if(!bgNextSlave)
			{
				bgNextSlave = slave;
				return true;
			}
			return false;
		}
		
		bool InterSYNTHESESlaveUpdateService::bgRTProcessSlave(
			const boost::shared_ptr<InterSYNTHESESlave> &slave
		) const
		{
			boost::mutex::scoped_lock(bgRTMutex);
			if(!bgRTNextSlave)
			{
				bgRTNextSlave = slave;
				return true;
			}
			return false;
		}

		void InterSYNTHESESlaveUpdateService::RunBackgroundUpdater()
		{
			bgUpdaterDone = false;
			boost::shared_ptr<InterSYNTHESESlave> slave;

			while(!bgUpdaterDone)
			{
				ServerModule::SetCurrentThreadRunningAction();
				{
					boost::mutex::scoped_lock(bgMutex);
					slave = bgNextSlave;
					bgNextSlave.reset();
				}
				if(slave.get())
				{
					try
					{
						slave->processFullUpdate();
					}
					catch(synthese::Exception& e)
					{
						Log::GetInstance().warn("Exception in Inter-SYNTHESE full update process", e);
					}

					// Record the request as slave activity
					// Even if the slave crashed and don't get the results,
					// we won't rebuild the full database on the new slave_update
					slave->markAsUpToDate();

					slave.reset();
				}
				
				{
					boost::mutex::scoped_lock(bgRTMutex);
					slave = bgRTNextSlave;
					bgRTNextSlave.reset();
				}
				if(slave.get())
				{
					try
					{
						slave->processFullRTUpdate();
					}
					catch(synthese::Exception& e)
					{
						Log::GetInstance().warn("Exception in Inter-SYNTHESE full Real Time update process", e);
					}
					
					slave->markAsUpToDate();
					
					slave.reset();
				}

				ServerModule::SetCurrentThreadWaiting();
				boost::this_thread::sleep(boost::posix_time::milliseconds(500));
			}
		}

}	}
