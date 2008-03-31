
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

#include "PhysicalStop.h"

#include <string>
#include <iostream>

#include "02_db/SQLiteRegistryTableSyncTemplate.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;

		/** PhysicalStop SQLite table synchronizer.
			@ingroup m15LS refLS
			
			Physical stops table :
				- on insert : 
				- on update : 
				- on delete : X

				@todo Use load / get / replace 
		*/
		class PhysicalStopTableSync : public db::SQLiteRegistryTableSyncTemplate<PhysicalStopTableSync,PhysicalStop>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_PLACEID;
			static const std::string COL_X;
			static const std::string COL_Y;
			static const std::string COL_OPERATOR_CODE;

			PhysicalStopTableSync ();
			~PhysicalStopTableSync ();
		};
	}
}

#endif
