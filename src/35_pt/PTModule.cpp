
/** PTModule class implementation.
    @file PTModule.cpp

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

#include "ServerModule.h"
#include "PTModule.h"
#include "ScheduledService.h"
#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace env;
	

	namespace graph
	{
		template<> const GraphIdType GraphModuleTemplate<PTModule>::GRAPH_ID(2);
	}

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,PTModule>::FACTORY_KEY("35_pt");
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<PTModule>::NAME("Transport public");
		
		template<> void ModuleClassTemplate<PTModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<PTModule>::Init()
		{
			shared_ptr<thread> theThread(
				new thread(
					&PTModule::RTDataCleaner
			)	);
			ServerModule::AddThread(theThread, "Real time data cleaner");
		}
		
		template<> void ModuleClassTemplate<PTModule>::End()
		{
		}
	}

	namespace pt
	{
		void PTModule::RTDataCleaner()
		{
			Registry<ScheduledService>& registry(
				Env::GetOfficialEnv().getEditableRegistry<ScheduledService>()
			);

			while(true)
			{
				ServerModule::SetCurrentThreadRunningAction();

				posix_time::ptime now(posix_time::second_clock::local_time());

				BOOST_FOREACH(Registry<ScheduledService>::value_type& service, registry)
				{
					if(now > service.second->getNextRTUpdate())
					{
						service.second->clearRTData();
					}
				}

				ServerModule::SetCurrentThreadWaiting();

				this_thread::sleep(posix_time::minutes(1));
			}
		}
	}
}