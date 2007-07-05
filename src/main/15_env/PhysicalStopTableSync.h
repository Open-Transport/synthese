
/** PhysicalStopTableSync class header.
	@file PhysicalStopTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H
#define SYNTHESE_ENVLSSQL_PHYSICALSTOPTABLESYNC_H

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;

		/** PhysicalStop SQLite table synchronizer.
			@ingroup m15
			
			Physical stops table :
				- on insert : 
				- on update : 
				- on delete : X

				@todo Use load / get / replace 
		*/
		class PhysicalStopTableSync : public db::SQLiteTableSyncTemplate<PhysicalStop>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_PLACEID;
			static const std::string COL_X;
			static const std::string COL_Y;
			static const std::string COL_OPERATOR_CODE;

			PhysicalStopTableSync ();
			~PhysicalStopTableSync ();

		protected:

			/** Action to do on BroadcastPoint creation.
			This method loads a new object in ram.
			*/
			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows, bool isFirstSync = false);

			/** Action to do on BroadcastPoint creation.
			This method updates the corresponding object in ram.
			*/
			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			/** Action to do on BroadcastPoint deletion.
			This method deletes the corresponding object in ram and runs 
			all necessary cleaning actions.
			*/
			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);
		};
	}
}

#endif
