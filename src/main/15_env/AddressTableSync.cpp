
/** AddressTableSync class implementation.
	@file AddressTableSync.cpp

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

#include <sqlite/sqlite3.h>
#include <assert.h>

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/AddressTableSync.h"
#include "15_env/Address.h"

using namespace boost;

using synthese::util::Conversion;
using synthese::db::SQLiteResult;

namespace synthese
{
	using namespace db;

	namespace env
	{

			AddressTableSync::AddressTableSync ()
				: ComponentTableSync (ADDRESSES_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (ADDRESSES_TABLE_COL_PLACEID, "INTEGER", false);
			addTableColumn (ADDRESSES_TABLE_COL_ROADID, "INTEGER", false);
			addTableColumn (ADDRESSES_TABLE_COL_METRICOFFSET, "DOUBLE", false);
			addTableColumn (ADDRESSES_TABLE_COL_X, "DOUBLE", true);
			addTableColumn (ADDRESSES_TABLE_COL_Y, "DOUBLE", true);

		}



		AddressTableSync::~AddressTableSync ()
		{

		}

		    


		void 
		AddressTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, db::TABLE_COL_ID)));
		    
			if (environment.getAddresses ().contains (id)) return;

			shared_ptr<Address> address(
			new synthese::env::Address (
						id,
						environment.getConnectionPlaces ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_PLACEID))).get(),
						environment.getRoads ().get (Conversion::ToLongLong (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_ROADID))).get(),
						Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_METRICOFFSET)),
						Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)),
						Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y))
						))
			;
		    

			environment.getAddresses ().add (address);
		}



		void 
		AddressTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
			shared_ptr<Address> address = environment.getAddresses ().getUpdateable (id);
			address->setX (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_X)));
			address->setY (Conversion::ToDouble (rows.getColumn (rowIndex, ADDRESSES_TABLE_COL_Y)));
		}



		void 
		AddressTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
					synthese::env::Environment& environment)
		{
			uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
			environment.getAddresses ().remove (id);
		}













	}

}

