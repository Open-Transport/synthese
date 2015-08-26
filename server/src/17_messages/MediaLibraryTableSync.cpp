/** MediaLibraryTableSync class implementation.
	@file MediaLibraryTableSync.cpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "MediaLibraryTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace messages;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync, MediaLibraryTableSync>::FACTORY_KEY("17.50 Media Libraries");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<MediaLibraryTableSync>::TABLE(
			"t122_media_libraries"
			);

		template<> const Field DBTableSyncTemplate<MediaLibraryTableSync>::_FIELDS[] = { Field() }; // Defined by the record

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<MediaLibraryTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<MediaLibraryTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
			){
			//TODO Check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<MediaLibraryTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
			){
		}



		template<> void DBTableSyncTemplate<MediaLibraryTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
			){
		}



		template<> void DBTableSyncTemplate<MediaLibraryTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
			){
			//TODO Log the removal
		}
	}

}
