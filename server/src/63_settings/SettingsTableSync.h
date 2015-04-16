
//////////////////////////////////////////////////////////////////////////
///	SettingsTableSync class header.
///	@file SettingsTableSync.h
///	@author Hugues
///	@date 2010
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

#ifndef SYNTHESE_SettingsTableSync_h__
#define SYNTHESE_SettingsTableSync_h__

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace settings
	{

		/** A setting value

		  This simple class represents a setting has stored in the database.

		  **/
		class SettingsValue:
			public virtual util::Registrable
		{
		public:

			//! The setting's proprietary module
			std::string module;

			//! The setting's name and value
			std::string name;
			std::string value;

			// Chosen registry class.
			typedef util::Registry<SettingsValue> Registry;

			// Constructor for Registrable
			SettingsValue(util::RegistryKeyType key = 0);

			// Setting to string
			std::string ToString();
		};

		//!	Settings table synchronizer.
		class SettingsTableSync:
			public db::DBDirectTableSyncTemplate<
				SettingsTableSync,
				SettingsValue,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:

			//! @name Field names
			//@{
				static const std::string COL_NAME;
				static const std::string COL_MODULE;
				static const std::string COL_VALUE;
			//@}

		};
	}
}

#endif // SYNTHESE_SettingsTableSync_h__
