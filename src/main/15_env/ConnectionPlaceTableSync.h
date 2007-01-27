
/** ConnectionPlaceTableSync class header.
	@file ConnectionPlaceTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H
#define SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H


#include <string>
#include <iostream>

#include "ComponentTableSync.h"



namespace synthese
{

namespace env
{
	class ConnectionPlace;

/** ConnectionPlace SQLite table synchronizer.
	@ingroup m15
*/
class ConnectionPlaceTableSync : public ComponentTableSync
{
 public:

    ConnectionPlaceTableSync ();
    ~ConnectionPlaceTableSync ();

 protected:

    void doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		synthese::env::Environment& target);

    void doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
		    synthese::env::Environment& target);

    void doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
		   synthese::env::Environment& target);


 private:

};



/** Connection places table :
- on insert : 
- on update : 
- on delete : X
*/
static const std::string CONNECTIONPLACES_TABLE_NAME ("t007_connection_places");
static const std::string CONNECTIONPLACES_TABLE_COL_NAME ("name");
static const std::string CONNECTIONPLACES_TABLE_COL_CITYID ("city_id");
static const std::string CONNECTIONPLACES_TABLE_COL_CONNECTIONTYPE ("connection_type");
static const std::string CONNECTIONPLACES_TABLE_COL_ISCITYMAINCONNECTION ("is_city_main_connection");
static const std::string CONNECTIONPLACES_TABLE_COL_DEFAULTTRANSFERDELAY ("default_transfer_delay");
static const std::string CONNECTIONPLACES_TABLE_COL_TRANSFERDELAYS ("transfer_delays");
static const std::string CONNECTIONPLACES_TABLE_COL_ALARMID ("alarm_id");



}

}
#endif

