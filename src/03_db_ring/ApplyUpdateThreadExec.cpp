#include "03_db_ring/ApplyUpdateThreadExec.h"
#include "03_db_ring/Constants.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"
#include "03_db_ring/UpdateRecordTableSync.h"

#include "01_util/Log.h"

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


using namespace boost::posix_time;


using namespace synthese::util;



namespace synthese
{
    namespace dbring
    {

	ApplyUpdateThreadExec::ApplyUpdateThreadExec (
	    //boost::shared_ptr<boost::recursive_mutex>& applyMutex,
	    const LatePendingPolicy& latePendingPolicy)

	    : /*_applyMutex (applyMutex)
		, */ _latePendingPolicy (latePendingPolicy)
	{
	}
	


	void 
	ApplyUpdateThreadExec::finalize ()
	{
	}


	void 
	ApplyUpdateThreadExec::loop ()
	{
	    // boost::recursive_mutex::scoped_lock lock (*_applyMutex);


	    // All pending records are loaded chronologically
	    // (oldest first).

	    const bool beforeLastAcknowledged (true);
	    const bool afterLastAcknowledged (false);

	    const bool withBlobs (true);
	    const bool withoutBlobs (false);

	    const boost::posix_time::ptime lastAcknowledgedTimestamp = DbRingModule::GetNode ()->getLastAcknowledgedTimestamp ();

	    // First, late pending records.
	    if (_latePendingPolicy == IMMEDIATE)
	    {
		// SQL will be executed so load fully all records.
		std::vector<UpdateRecordSPtr> urv;
		
		UpdateRecordTableSync::LoadPendingRecords (urv, lastAcknowledgedTimestamp, beforeLastAcknowledged, withBlobs);
		
		for (std::vector<UpdateRecordSPtr>::iterator it = urv.begin ();
		     it != urv.end (); ++it)
		{
		    UpdateRecordSPtr ur = *it;
		    UpdateRecordTableSync::ApplyUpdateRecord (ur);
		}
	    }
	    else if (_latePendingPolicy == POSTPONE)
	    {
		std::vector<UpdateRecordSPtr> urv;
		UpdateRecordTableSync::LoadPendingRecords (urv, lastAcknowledgedTimestamp, beforeLastAcknowledged, withoutBlobs);
		
		for (std::vector<UpdateRecordSPtr>::iterator it = urv.begin ();
		     it != urv.end (); ++it)
		{
		    UpdateRecordSPtr ur = *it;
		    UpdateRecordTableSync::PostponeUpdateRecord (ur);
		}

	    }
	    else if (_latePendingPolicy == ABORT)
	    {
		// Do not load SQL data because update record will be aborted anyway.
		std::vector<UpdateRecordSPtr> urv;
		UpdateRecordTableSync::LoadPendingRecords (urv, lastAcknowledgedTimestamp, beforeLastAcknowledged, withoutBlobs);
		
		for (std::vector<UpdateRecordSPtr>::iterator it = urv.begin ();
		     it != urv.end (); ++it)
		{
		    UpdateRecordSPtr ur = *it;
		    UpdateRecordTableSync::AbortUpdateRecord (ur);
		}
	    }

	    // Second, other pending records.
	    std::vector<UpdateRecordSPtr> urv;
	    UpdateRecordTableSync::LoadPendingRecords (urv, lastAcknowledgedTimestamp, afterLastAcknowledged, withBlobs);
	    // std::cerr << "<< Found " << urv.size () << " pending records" << std::endl;
	    for (std::vector<UpdateRecordSPtr>::iterator it = urv.begin ();
		 it != urv.end (); ++it)
	    {
		UpdateRecordSPtr ur = *it;
		UpdateRecordTableSync::ApplyUpdateRecord (ur);
	    }

	}

	




    }    
}

