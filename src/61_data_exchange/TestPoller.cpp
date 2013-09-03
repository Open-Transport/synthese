
/** TestPoller class implementation.
	@file TestPoller.cpp

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

#include "TestPoller.hpp"

#include "PermanentThread.hpp"
#include "ServerModule.h"

#include <boost/thread.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace data_exchange;
	using namespace server;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Device, TestPoller>::FACTORY_KEY = "test_poller";
	}

	namespace data_exchange
	{
		const std::string TestPoller::Poller_::PARAMETER_TEST_TO_DISPLAY("test_to_display");



		bool TestPoller::Poller_::launchPoller(
		) const {
			
			// Launch the thread and returns true
			ServerModule::AddThread(boost::bind(&TestPoller::Poller_::startDisplay, this), "Test poller");

			return true;
		}



		TestPoller::Poller_::Poller_(
			util::Env& env,
			const server::PermanentThread& permanentThread,
			util::ParametersMap& pm
		):	Poller(env, permanentThread, pm)
		{}



		util::ParametersMap TestPoller::Poller_::getParametersMap() const
		{
			ParametersMap map;

			if(!_testToDisplay.empty())
			{
				map.insert(PARAMETER_TEST_TO_DISPLAY, _testToDisplay);
			}

			return map;
		}



		void TestPoller::Poller_::setFromParametersMap(const util::ParametersMap& map)
		{
			_testToDisplay = map.getDefault<string>(PARAMETER_TEST_TO_DISPLAY, "test poller par defaut");
		}

		void TestPoller::Poller_::startDisplay() const
		{
			while (true)
			{
				this_thread::sleep(boost::posix_time::seconds(5));
				Log::GetInstance().info(_testToDisplay);
			}
		}
	}
}
