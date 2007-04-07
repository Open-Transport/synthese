
/** SiteTableSync class header.
	@file SiteTableSync.h

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

#ifndef SYNTHESE_SiteTableSync_H__
#define SYNTHESE_SiteTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSync.h"

namespace synthese
{
	namespace routeplanner
	{

		/** InterfaceTableSync SQLite table synchronizer.
			@ingroup m33
		*/

		class SiteTableSync : public db::SQLiteTableSyncTemplate<Site>
		{
		private:
			static const std::string TABLE_COL_ID;
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_START_DATE;
			static const std::string TABLE_COL_END_DATE;
			static const std::string TABLE_COL_ONLINE_BOOKING;
			static const std::string TABLE_COL_USE_OLD_DATA;

		public:

			/** Site SQLite table constructor.
			*/
			SiteTableSync();
			~SiteTableSync ();

		protected:

			void rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

			void rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResult& rows);

		};

	}
}
#endif // SYNTHESE_SiteTableSync_H__
