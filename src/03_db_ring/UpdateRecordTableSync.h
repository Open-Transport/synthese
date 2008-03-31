
/** UpdateRecordTableSync class header.
	@file UpdateRecordTableSync.h

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

#ifndef SYNTHESE_DBRING_UPDATERECORDTABLESYNC_H
#define SYNTHESE_DBRING_UPDATERECORDTABLESYNC_H


#include "02_db/SQLiteDirectTableSyncTemplate.h"
#include "03_db_ring/UpdateRecord.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace synthese
{

namespace dbring
{





class UpdateRecordTableSync : public db::SQLiteDirectTableSyncTemplate<UpdateRecordTableSync,UpdateRecord>
{


public:

    static const std::string TABLE_COL_TIMESTAMP;
    static const std::string TABLE_COL_EMITTERNODEID;
    static const std::string TABLE_COL_STATE;
    static const std::string TABLE_COL_SQL;

    UpdateRecordTableSync ();
    ~UpdateRecordTableSync ();


    virtual void rowsAdded (db::SQLite* sqlite, 
			    db::SQLiteSync* sync,
			    const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

    virtual void rowsUpdated (db::SQLite* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResultSPtr& rows);

    virtual void rowsRemoved (db::SQLite* sqlite, 
			      db::SQLiteSync* sync,
			      const db::SQLiteResultSPtr& rows);

    static void LoadDeltaUpdate (UpdateRecordSet& dest, 
				 const boost::posix_time::ptime& clientLastAcknowledgedTimestamp,
				 std::set<uid> clientLastPendingIds);

    static void LoadPendingRecordIds (std::set<uid>& updateRecordIds);

    static void LoadPendingRecords (std::vector<UpdateRecordSPtr>& updateRecords, 
				    const boost::posix_time::ptime& lastAcknowledgedTimestamp,
				    bool lateOnes, bool withBlob);


    static void ApplyUpdateRecord (const UpdateRecordSPtr& updateRecord, bool overwriteTimestamp = false);

    static void AbortUpdateRecord (const UpdateRecordSPtr& updateRecord, bool overwriteTimestamp = false);

    static void PostponeUpdateRecord (const UpdateRecordSPtr& updateRecord);


    /**
       Select all update records between two timestamps.
       @param startTimestamp Start timestamp (exclusive)
       @param endTimestamp End timestamp (exclusive)
       @param result Result vector
    */
    /*   static void SelectAllRecordIdsBetween (const boost::posix_time::ptime& startTimestamp,
					   const boost::posix_time::ptime& endTimestamp,
					   std::vector<uid>& result); 
	
    */


    static boost::posix_time::ptime GetLastTimestampWithState (const RecordState& recordState);

    static long GetLastUpdateIndex (NodeId nodeId);

private:


};



}
}



#endif



