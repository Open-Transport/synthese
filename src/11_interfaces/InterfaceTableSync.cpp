
/** InterfaceTableSync class implementation.
	@file InterfaceTableSync.cpp

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

#include "Conversion.h"

#include "SQLiteResult.h"

#include "InterfaceTableSync.h"

using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const std::string FactorableTemplate<SQLiteTableSync,InterfaceTableSync>::FACTORY_KEY("16.01 Interfaces");
	}

	namespace interfaces
	{
		const std::string InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE = "no_session_default_page";
		const std::string InterfaceTableSync::TABLE_COL_NAME = "name";
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<InterfaceTableSync>::TABLE(
			InterfaceTableSync::CreateFormat(
				"t024_interfaces",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE, TEXT),
					SQLiteTableFormat::Field(InterfaceTableSync::TABLE_COL_NAME, TEXT),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::Indexes()
		)	);

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Load(
			Interface* interf,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			interf->setKey(rows->getLongLong ( TABLE_COL_ID));
			interf->setNoSessionDefaultPageCode(rows->getText ( InterfaceTableSync::TABLE_COL_NO_SESSION_DEFAULT_PAGE));
			interf->setName(rows->getText ( InterfaceTableSync::TABLE_COL_NAME));
		}


		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Unlink(
			Interface* interf,
			Env* env
		){

		}

		template<> void SQLiteDirectTableSyncTemplate<InterfaceTableSync,Interface>::Save(Interface* interf)
		{
			/// @todo Implementation
		}
	}

	namespace interfaces
	{
		InterfaceTableSync::InterfaceTableSync()
			: db::SQLiteRegistryTableSyncTemplate<InterfaceTableSync,Interface> ()
		{
		}
	}
}



