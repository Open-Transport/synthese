
/** PermanentThread class implementation.
	@file PermanentThread.cpp

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

#include "PermanentThread.hpp"

#include "Env.h"
#include "Device.h"
#include "Poller.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	CLASS_DEFINITION(server::PermanentThread, "t115_permanent_thread", 115)
	FIELD_DEFINITION_OF_OBJECT(server::PermanentThread, "permanent_thread_id", "permanent_thread_ids")


	FIELD_DEFINITION_OF_TYPE(DeviceKey, "device", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Documentation, "documentation", SQL_TEXT)

	namespace server
	{
		PermanentThread::PermanentThread(
			util::RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<PermanentThread, PermanentThreadRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(DeviceKey),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters),
					FIELD_VALUE_CONSTRUCTOR(Active, true),
					FIELD_DEFAULT_CONSTRUCTOR(Documentation)
			)	)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Poller creation helper
		/// @param env the permanent_thread environment
		boost::shared_ptr<Poller> PermanentThread::getPoller(
			util::Env& env,
			util::ParametersMap& pm
		) const {
			boost::shared_ptr<Device> device(Factory<Device>::create(get<DeviceKey>()));
			return device->getPoller(env, *this, pm);
		}


		void PermanentThread::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
		}



		void PermanentThread::unlink()
		{
			// Delete the poller cache in case of parameter update
			_poller.reset();
		}



		void PermanentThread::launch() const
		{
			ParametersMap pm;
			if(!_poller.get())
			{
				boost::shared_ptr<Device> device(Factory<Device>::create(get<DeviceKey>()));
				_pollerEnv.reset(new Env);
				_poller = device->getPoller(
					*_pollerEnv,
					*this,
					pm
				);
				_poller->setFromParametersMap(get<Parameters>());
			}

			_pollerEnv->clear();

			_poller->launchPoller();
		}



		void PermanentThread::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix
		) const	{

			map.merge(get<Parameters>());
		}
}	}

