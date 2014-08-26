/** SettingsModule class header.
	@file SettingsModule.hpp

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

#include "Settings.h"

#include "SettingsTableSync.h"

#include "Env.h"
#include "Log.h"
#include "DBTransaction.hpp"

namespace synthese
{
	namespace settings
	{

		using namespace synthese::util;

		Settings* Settings::_instance = new Settings();

		Settings& Settings::GetInstance()
		{
			return *_instance;
		}


		// !!! The Get, Init and Set functions are defined in the Settings.h file


		bool Settings::Exists (
			const std::string& module,
			const std::string& name
		) const
		{
			return ( _idForSetting(module,name) != 0 );
		}


		// Add the pointer to the multimap
		void Settings::Register (
			const std::string& module,
			const std::string& name,
			Settable* listener
		)
		{
			_listeners.insert(std::pair<std::string,Settable*>( _keyForSetting(module,name), listener ));
		}


		// Remove the pointer to the multimap
		void Settings::Unregister (
			const Settable* listener,
			const std::string& module,
			const std::string& name
		)
		{

			ListenerMap::iterator first;
			ListenerMap::iterator last;

			// If no name or module is given, search everywhere
			if ( name.empty() || module.empty() )
			{
				first = _listeners.begin();
				last = _listeners.end();
			}
			else // Search the same value/id
			{
				first = _listeners.lower_bound( _keyForSetting(module,name) );
				last = _listeners.upper_bound( _keyForSetting(module,name) );
			}

			// Loop through the results and erase any valid occurence
			while ( first != last )
			{
				if ( (*first).second == listener )
				{
					_listeners.erase(first);
				}

				first++;
			}
		}


		// Simple, really, simple
		// This is just to be sure the key is formed in the same way everywhere
		std::string Settings::_keyForSetting (
			const std::string& module,
			const std::string& name
		) const
		{
			return module + "." + name;
		}


		// Simple search (loop through) of the environment
		util::RegistryKeyType Settings::_idForSetting (
			const std::string& module,
			const std::string& name
		) const
		{
			// This is why I would love to simply used the database.
			// While I know this is possible, I also know that it's not "the way" of doing things.
			// And so we loop through. Talk about not not optimized.
			// Rant by Bastien Noverraz (TL)
			BOOST_FOREACH(Registry<SettingsValue>::value_type it, util::Env::GetOfficialEnv().getRegistry<SettingsValue>())
			{
				// Check the module + name
				if (it.second->name == name && it.second->module == module)
				{
					return it.first;
				}
			}

			// Not found, sorry
			return -0;
		}
	}
}
