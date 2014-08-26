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

#ifndef SYNTHESE_Settings_H__
#define SYNTHESE_Settings_H__

#include "SettingsTableSync.h"

#include "Settable.h"
#include "Env.h"

#include <string>
#include <map>

namespace synthese
{
	namespace settings
	{

		/** Settings access

		  This class enable any object to access the settings.
		  This class is a singleton.

		  <h2>Storage</h2>

		  The settings are stored in the database through SettingsTableSync.
		  This object uses it to fetch and save the settings permanantly.

		  <h2>A setting</h2>

		  Each setting has a name and a value, both represented by a string.
		  In addition, each setting is in a module, to encapsulate them and avoid name collision.

		  <h2>Security</h2>

		  No security is applied, any object can get/set setting for another one.

		  <h2>Module</h2>

		  The "module" parameter used in this object can be anything and might not corresponds to an object's module name.

		  <h2>Registering and notify<h2>

		  To enable an object to react to one of its settings change, a notify callback system is provided.
		  An object willing to be called on a setting update have to implement the Settable interface.
		  Then, to be called, the object must register itself to Settings with the register() function.

		  On a setting update, its Settable::ValueUpdated() fonction will be called.

		  **/
		class Settings
		{
		public:

			//! Get the unique Settings instance
			static Settings& GetInstance ();

			/** Get a setting

			  If the settings is not found, return the default value.

			  @param module The setting module (parent)
			  @param name The setting name
			  @param defaultValue A default value to return if the setting is not found
			  @return The setting's value
			  **/
			template<typename T>
			T Get (
				const std::string& module,
				const std::string& name,
				const T& defaultValue = T()
			) const;

			/** Init a setting

			  If the settings is not found, set it and return its value.
			  All registered listener will be called, even if the setting is already there.
			  The notify flag will be set at false.

			  @param module The setting module (parent)
			  @param name The setting name
			  @param defaultValue A default value to return if the setting is not found
			  @return The setting's value
			  **/
			template<typename T>
			T Init (
				const std::string& module,
				const std::string& name,
				const T& defaultValue = T()
			);

			/** Set a setting

			  If the value exists, it will be replaced.

			  @param module The setting module (parent)
			  @param name The setting name
			  @param value The value to set
			  @param notify Notify the listener (added through Register()), true by default
			  **/
			template<typename T>
			void Set (
				const std::string& module,
				const std::string& name,
				const T& value,
				bool notify = true
			);

			/** Does a setting exists?
			  @param module The setting module (parent)
			  @param name The setting name
			  @return True is the setting is found, false otherwise
			  **/
			bool Exists (
				const std::string& module,
				const std::string& name
			) const;

			/** Register a listener

			  This function will save the object linked with this setting.
			  If the given setting is modified, the object's ValueUpdated() function will be called.

			  Before and object is deleted it must be unregistered, else it might be called while deleted.

			  @param module The setting module
			  @param name The setting name
			  @param listener The object listening on this setting changes
			  **/
			void Register (
				const std::string& module,
				const std::string& name,
				synthese::settings::Settable* listener
			);

			/** Unregister a listener

			  Will stop all calls to ValueUpdated for this object and remove its pointer from the Settings.
			  This has no effect on the value of the setting itself.

			  If the setting name or module is not specified, all references to this object will be removed.

			  @param listener The object listening on this setting changes
			  @param module The setting module (or nothing)
			  @param name The setting name (or nothing)
			  **/
			void Unregister (
				const Settable* listener,
				const std::string& module = std::string(),
				const std::string& name = std::string()
			);

		private:
			// Unique instance
			static Settings* _instance;

			// Map of the listener for each settings
			typedef std::multimap<std::string,Settable*> ListenerMap;
			ListenerMap _listeners;

			// Constructor and destructor
			Settings () {}
			~Settings () {}

			// Copy is not allowed
			Settings& operator= (const Settings&) { return *this; }
			Settings (const Settings&) { }

			// Returns the full name of a setting from its module and name
			std::string _keyForSetting (
				const std::string& module,
				const std::string& name
			) const;

			// Returns the ID of a specific setting in the DB
			util::RegistryKeyType _idForSetting (
				const std::string& module,
				const std::string& name
			) const;

		};



		//---------------------------- IMPLEMENTATION -----------------------------
		// This function is put here because of its templated type
		// C++ requires you to have it in the .h because of all the template it generates

		// Fetch the data from the environement
		template<typename T>
		T Settings::Get (
			const std::string& module,
			const std::string& name,
			const T& defaultValue
		) const
		{
			// Search for the value
			util::RegistryKeyType key = _idForSetting(module,name);

			// Invalid ID, return the default value
			if (key)
			{
				// Fetch it
				boost::shared_ptr<SettingsValue> sv = util::Env::GetOfficialEnv().getEditable<SettingsValue>(key);

				// Try to convert it and return the default value if it fails
				try
				{
					return boost::lexical_cast<T>(sv.get()->value);
				}
				catch (...)
				{
					util::Log::GetInstance().error("Settings : error while converting setting " + sv.get()->ToString() + ", using default value");
				}
			}

			return defaultValue;
		}


		// Fetch the data from the environement and if not found add it
		template<typename T>
		T Settings::Init (
			const std::string& module,
			const std::string& name,
			const T& defaultValue
		)
		{
			// Search for the value
			util::RegistryKeyType key = _idForSetting(module,name);

			T value = defaultValue;

			// Invalid ID, add the value
			if ( ! key )
			{
				Set<T>(module,name,defaultValue,false);
			}
			else // The value exists, load it
			{
				value = Get<T>(module,name,defaultValue);
			}

			return value;
		}

		// Save the value in the environment and notify any listener
		template<typename T>
		void Settings::Set (
			const std::string& module,
			const std::string& name,
			const T& value,
			bool notify
		)
		{
			// Try to convert the value
			std::string strValue;
			try
			{
				strValue = boost::lexical_cast<std::string>(value);
			}
			catch (...)
			{
				util::Log::GetInstance().error("Settings : error while converting setting value in string for " + _keyForSetting(module,name) + ", value not saved");
				return;
			}

			// Search for it in the DB
			util::RegistryKeyType key = _idForSetting(module,name);

			boost::shared_ptr<SettingsValue> sv;

			// Not found, create a new one
			if (!key)
			{
				sv = boost::shared_ptr<SettingsValue>(new SettingsValue( SettingsTableSync::getId() ));
				sv->module = module;
				sv->name = name;
				sv->value = strValue;
				util::Env::GetOfficialEnv().getEditableRegistry<SettingsValue>().add(sv);
				util::Log::GetInstance().debug("Settings : Add " + sv->ToString());
			}
			else // Found, replace it
			{
				sv = util::Env::GetOfficialEnv().getEditable<SettingsValue>(key);
				sv->value = strValue;
				util::Env::GetOfficialEnv().getEditableRegistry<SettingsValue>().replace(sv);
				util::Log::GetInstance().debug("Settings : Set " + sv->ToString());
			}

			// Save it
			db::DBTransaction transaction;
			SettingsTableSync::Save(sv.get(),transaction);
			transaction.run();

			// Call the listeners
			ListenerMap::const_iterator first = _listeners.lower_bound( _keyForSetting(module,name) );
			ListenerMap::const_iterator last = _listeners.upper_bound( _keyForSetting(module,name) );
			while (first != last)
			{
				(*first).second->ValueUpdated(module,name,strValue,notify);
				first++;
			}
		}
	}
}

#endif // SYNTHESE_Settings_H__
