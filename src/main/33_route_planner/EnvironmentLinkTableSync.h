
/** EnvironmentLinkTableSync class header.
	@file EnvironmentLinkTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_ENVIRONMENTLINKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_ENVIRONMENTLINKTABLESYNC_H

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace routeplanner
	{
		class SiteCommercialLineLink;

		/** Synchronizer for environment link tables.
		    
			This class holds a static mapping of classId <=> tableName to be maintained
			each time a new component registry is added to the environment.
			This synchronizer MUST be registered after all component synchronizers so that the mapping
			is complete.

			@ingroup m33
		*/

		class EnvironmentLinkTableSync : public db::SQLiteTableSyncTemplate<SiteCommercialLineLink>
		{
		public:
			static const std::string COL_SITE_ID;
			static const std::string COL_COMMERCIAL_LINE_ID;

		 protected:

			EnvironmentLinkTableSync ();

			void rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
					synthese::db::SQLiteSync* sync,
					const synthese::db::SQLiteResult& rows, bool isFirstSync = false);

			void rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
					   synthese::db::SQLiteSync* sync,
					   const synthese::db::SQLiteResult& rows);

			void rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
					  synthese::db::SQLiteSync* sync,
					  const synthese::db::SQLiteResult& rows);

		};
	}
}

#endif
