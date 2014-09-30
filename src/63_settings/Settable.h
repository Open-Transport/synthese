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

#ifndef SYNTHESE_Settable_H__
#define SYNTHESE_Settable_H__

#include <string>

namespace synthese
{
	namespace settings
	{

		/** Settable template

		  This class represents an object whose settings are notifyable.

		  This means that when a setting registered using Settings::Register() is modified,
		  this class's ValueUpdated() function will be called with the newly setted value.

		  The value itself will be stored by Settings before this function is called.

		  **/
		class Settable
		{
		public:

			/** A setting has been updated

			  A setting registered with Settings::Register() has been updated.

			  This function must be implemented.

			  A note on the notify parameter : if set to false, this indicates that the object that set the setting
			  asked for the listener not to notify its change.
			  This might mean anything, but generaly it is used to set multiple settings but to take action on their change only
			  when the last one is set.

			  @param module The setting module (parent)
			  @param name The setting name
			  @param value The value updated
			  @param notify False if the object that set the value asked not to notify the change
			  **/
			virtual void ValueUpdated (
				const std::string& module,
				const std::string& name,
				const std::string& value,
				bool notify
			) = 0;

		};
	}
}

#endif // SYNTHESE_Settable_H__
