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

#ifndef SYNTHESE_SettingsModule_H__
#define SYNTHESE_SettingsModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{

	/** Settings module

	  Store settings in the database and gives access to them using an object (Settings)
	  or through web services (SettingsFunction, function "settings").

	  The table used to store the values is "t120_settings".

	  A setting has a module, a name and a value.
	  A module should use only its own settings, but any module can access any settings.

	  @{
	  **/


	//////////////////////////////////////////////////////////////////////////
	/// 63 Settings Module namespace.
	///	@author Bastien Noverraz (TL)
	///	@date 2014
	//////////////////////////////////////////////////////////////////////////
	namespace settings
	{


		/** 63 Settings module class.
		*/
		class SettingsModule:
				public server::ModuleClassTemplate<SettingsModule>
		{
			friend class server::ModuleClassTemplate<SettingsModule>;

		public:

			/** Does nothing
			@{
			*/
			static void Start();
			static void End();
			static void PreInit();
			static void Init();
			static void InitThread();
			static void CloseThread();
			/** @} */
		};
		/** @} */
	}
}

#endif
