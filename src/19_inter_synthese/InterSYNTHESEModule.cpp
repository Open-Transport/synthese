
/** InterSYNTHESEModule class implementation.
	@file InterSYNTHESEModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "InterSYNTHESEModule.hpp"

#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlave.hpp"
#include "ServerModule.h"

#include <boost/thread.hpp>

using namespace boost::posix_time;

using namespace boost;

namespace synthese
{
	using namespace inter_synthese;
	using namespace server;
	using namespace util;
	
	template<> const std::string util::FactorableTemplate<ModuleClass, InterSYNTHESEModule>::FACTORY_KEY("19_inter_synthese");
	
	namespace inter_synthese
	{
		time_duration InterSYNTHESEModule::_interSYNTHESEWaitingTime(seconds(10));
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<InterSYNTHESEModule>::NAME("Inter-SYNTHESE");

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::Init()
		{
			// Synchronization
			shared_ptr<thread> theThread(
				new thread(
					&InterSYNTHESEModule::InterSYNTHESE
			)	);
			ServerModule::AddThread(theThread, "Inter-SYNTHESE master synchronization");
		}

		template<> void ModuleClassTemplate<InterSYNTHESEModule>::End()
		{
		}
	}

	namespace inter_synthese
	{
		void InterSYNTHESEModule::InterSYNTHESE()
		{
			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				posix_time::ptime now(posix_time::second_clock::local_time());

				BOOST_FOREACH(
					InterSYNTHESESlave::Registry::value_type& slave,
					Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESESlave>()
				){
					slave.second->send();
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(posix_time::minutes(1));
			}

		}



		void InterSYNTHESEModule::Enqueue( const std::string& interSYNTHESEType, const std::string& parameter )
		{
			if(parameter.find(InterSYNTHESEQueueTableSync::TABLE.NAME) != string::npos)
			{
				return;
			}
			BOOST_FOREACH(
				InterSYNTHESESlave::Registry::value_type& slave,
				Env::GetOfficialEnv().getEditableRegistry<InterSYNTHESESlave>()
			){
				slave.second->enqueue(interSYNTHESEType, parameter);
			}
		}
}	}

