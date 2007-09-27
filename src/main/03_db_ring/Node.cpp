#include "03_db_ring/Node.h"
#include "03_db_ring/Constants.h"
#include "03_db_ring/DbRingException.h"
#include "03_db_ring/RecvTokenThreadExec.h"
#include "03_db_ring/UpdateRecordTableSync.h"
#include "03_db_ring/UpdateChronologyException.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLite.h"
  
#include "01_util/Log.h"
#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"
#include "01_util/threads/ManagedThread.h"


#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace synthese::util;
using namespace synthese::db;

using namespace boost::posix_time;

extern int nbres;


namespace synthese
{
namespace dbring
{




Node::Node (const NodeId& id)
    : _id (id)
    , _ringNodes ()
    , _updateLog (new UpdateLog ())
    , _lastUpdateIndex (0)
    , _ringNodesMutex (new boost::recursive_mutex ())
    , _updateLogMutex (new boost::recursive_mutex ())
{

}

    

Node::~Node ()
{
}
    


const NodeId& 
Node::getId () const
{
    return _id;
}




bool 
Node::hasInfo (const NodeId& nodeId, const RingId& ringId) const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    RingNodes::const_iterator it = _ringNodes.find (ringId);
    if (it == _ringNodes.end ()) return false;
    return it->second->hasInfo (nodeId);
}



    
NodeInfo
Node::getInfo (const NodeId& nodeId, const RingId& ringId) const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    assert (hasInfo (nodeId, ringId));
    return _ringNodes.find (ringId)->second->getInfo (nodeId);
}





bool
Node::hasInfo (const RingId& ringId) const
{
    return hasInfo (_id, ringId);
}



NodeInfo 
Node::getInfo (const RingId& ringId) const
{
    return getInfo (_id, ringId);
}



void 
Node::flushUpdates ()
{
    _updateLog->flush ();
}




void 
Node::setNodeInfoCallback (const NodeInfo& nodeInfo)
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);

    RingNodes::iterator it = _ringNodes.find (nodeInfo.getRingId ());
    if (it == _ringNodes.end ()) 
    {
	if (nodeInfo.getNodeId () != _id)
	{
	    throw DbRingException ("Node " + Conversion::ToString (_id) + " is not identified on ring " + Conversion::ToString (nodeInfo.getRingId ()));
	}

	Log::GetInstance ().info ("Node " + Conversion::ToString (_id) + " is now identified on ring " + Conversion::ToString (nodeInfo.getRingId ())
				  + " as " + nodeInfo.getHost () + ":" + Conversion::ToString (nodeInfo.getPort ()));

	// Look if a new tcp service needs to be opened.
	if (_listenPorts.find (nodeInfo.getPort ()) == _listenPorts.end ())
	{
	    // Create a new managed thread listening on this port and pushing token in
	    // this node token queue.
	    RecvTokenThreadExec* recvTokenThreadExec = new RecvTokenThreadExec (nodeInfo.getPort (), _receivedTokens);
	    
	    std::string threadName ("node_tcp_" + Conversion::ToString (nodeInfo.getPort ()));
	    bool autorespawn (true);
	    ManagedThread* recvTokenThread = 
		new ManagedThread (recvTokenThreadExec, threadName, 100, autorespawn);
	    
	    _listenPorts.insert (nodeInfo.getPort ());
	}

	// Sets this node identity for a given ring.
	_ringNodes.insert (std::make_pair (nodeInfo.getRingId (), RingNodeSPtr (new RingNode (nodeInfo, _updateLog))));
    }
    else
    {
//	Log::GetInstance ().info ("Node " + Conversion::ToString (_id) + " is now identified on ring " + Conversion::ToString (ringId)
//				  + " as " + host + ":" + Conversion::ToString (port));

	it->second->setInfo (nodeInfo.getNodeId (), nodeInfo);

	// TODO : change tcp service
    }

}




void 
Node::setUpdateRecordCallback (const UpdateRecordSPtr& updateRecord)
{
    boost::recursive_mutex::scoped_lock callbackLock (*_ringNodesMutex);
    _updateLog->setUpdateRecord (updateRecord);
    
    // If this is an acknowledgement, release the lock taken on update record key just before
    // it was saved.

}
















void 
Node::initialize ()
{
    _lastUpdateIndex = UpdateRecordTableSync::getLastUpdateIndex (_id);

    if (_ringNodes.empty ())
    {
	Log::GetInstance ().warn ("This node is not connected to any ring; it will be writable by default.");
    }
    else
    {
	for (RingNodes::iterator it = _ringNodes.begin ();
	     it != _ringNodes.end (); ++it)
	{
	    RingNodeSPtr rn = it->second;
	    rn->initialize ();
	}
    }

}






sqlite3* 
Node::getHandle ()
{
    return DBModule::GetSQLite()->getHandle ();
}



db::SQLiteResultSPtr 
Node::execQuery (const SQLiteStatementSPtr& statement, bool lazy)
{
    if (SQLite::IsUpdateStatement (statement->getSQL ())) throw ("Not a query statement : " + statement->getSQL ());

    return DBModule::GetSQLite()->execQuery (statement, lazy); 
}




SQLiteResultSPtr 
Node::execQuery (const SQLData& sql, bool lazy)
{
    return DBModule::GetSQLite()->execQuery (sql, lazy);
}




void
Node::execUpdate (const SQLiteStatementSPtr& statement, bool asynchronous)
{
    execUpdate (statement->getSQL (), asynchronous);
}




void 
Node::execUpdate (const SQLData& sql, bool asynchronous)
{
    // If this is not an update statement, return directly
    if (SQLite::IsUpdateStatement (sql) == false)
    {
	Log::GetInstance ().warn ("Not an update statement, discarded : " + sql);
	return;
    }

    // Ensure that the update log cannot be modified both by main loop and by local
    // update action. This guarantees that the local update will not occur after token sending
    // and before update log flushing.
    boost::recursive_mutex::scoped_lock updateLogLock (*_updateLogMutex);

    if (canWrite () == false)
    {
	throw DbRingException ("Node is locked for writing");
    }

    // Use UTC time
    boost::posix_time::ptime now (boost::date_time::microsec_clock<ptime>::universal_time ());

    // Increment record update.
    ++_lastUpdateIndex;
    UpdateRecordSPtr urp (new UpdateRecord (encodeUpdateKey (_id, _lastUpdateIndex) , now,
					    _id, PENDING,
					    sql ));

    // TODO : wrong way to do this. because
    // 1) the node hangs til the update has finished and does not respond to another nodes nor propagate the info
    // 2) the synchronous stuff is wrong cos no loop is done til update record acknowledged, but acknoledgement cannot 
    //    be done if node is not looping.

    // should have another thread polling acknowledged update records and executing them.
    // with another state succeeded ?

    saveUpdateRecord (urp);
    
    if (asynchronous == false)
    {
	// If synchronous update is done, we must wait until the update record is acknowledged locally.
        // This will be notified asynchronously by ::setUpdateRecordCallback.
	while (true)
	{
	    // Note : if node is not authority, getting the acknowledgement can take a while...
	    // or just not happened (TODO : add a timeout ?)
	    RecordState state = _updateLog->getUpdateRecord (urp->getKey ())->getState ();
	    if (state == ACKNOWLEDGED) break;
	    if (state == FAILED) 
	    {
		throw DbRingException ("ring update failed");
	    }
	    
	    loop ();
	    Thread::Sleep (10);
	}

    }
}





void 
Node::loop ()
{
    boost::recursive_mutex::scoped_lock updateLogLock (*_updateLogMutex);

    TokenSPtr token;
    if (_receivedTokens.empty () == false) 
    {
	token = _receivedTokens.back ();
	_receivedTokens.pop ();
	mergeUpdateLog (token);
    }
	
    for (RingNodes::iterator it = _ringNodes.begin ();
	 it != _ringNodes.end (); ++it)
    {
	RingNodeSPtr rn = it->second;
	rn->loop (token);
    }
    
}




boost::posix_time::ptime 
Node::getLastPendingTimestamp () const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    if (_ringNodes.size () == 0) return min_date_time;

    // if node is identified at least on one ring, returns this ring node 
    // timestamp (same for all).
    return _ringNodes.begin ()->second->getLastPendingTimestamp ();
}




boost::posix_time::ptime 
Node::getLastAcknowledgedTimestamp () const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    if (_ringNodes.size () == 0) return min_date_time;

    // if node is identified at least on one ring, returns this ring node 
    // timestamp (same for all).
    return _ringNodes.begin ()->second->getLastAcknowledgedTimestamp ();
}





void
Node::mergeUpdateLog (const TokenSPtr& token)
{
    const UpdateRecordSet& records  = token->getUpdateLog ()->getUpdateRecords ();
    if (records.size () == 0) return;
    
    // Prepare local in-memory update log for merge
    UpdateRecordTableSync::loadAllAfterTimestamp (_updateLog, 
						  token->getUpdateLog ()->getUpdateLogBeginTimestamp (),
						  true //inclusive
	);    
    
    for (UpdateRecordSet::const_iterator itr = records.begin ();
	 itr != records.end (); ++itr)
    {
	UpdateRecordSPtr ur = *itr;

	// Do not propagate failed update record.
	if (ur->getState () == FAILED) continue;

	// Do not propagate locally acknowledged update record (authority might not have got the
	// info that local node has already acknowledged it).
	if (_updateLog->hasUpdateRecord (ur->getKey ()) &&
	    _updateLog->getUpdateRecord (ur->getKey ())->getState () == ACKNOWLEDGED) continue;

	std::stringstream ss;
	ss << "Merging record " << (*ur);
	Log::GetInstance ().debug (ss.str ());

	if (ur->getTimestamp () < getLastAcknowledgedTimestamp ())
	{
	    throw UpdateChronologyException ("Cannot insert an update record in past");
	}
	
	saveUpdateRecord (ur);
    }

}




bool 
Node::isMasterAuthority () const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);

    // By default, a node which is not connected to any other ring node is
    // a master authority.
    if (_ringNodes.empty ()) return true;

    for (RingNodes::const_iterator it = _ringNodes.begin (); it != _ringNodes.end (); ++it)
    {
	if (it->second->getInfo ().isAuthority () == false) return false;
    }
    return true;
}




void
Node::saveUpdateRecord (const UpdateRecordSPtr& urp)
{
    // Note : acknowledging an update record does NOT increment the record timestamp, but it
    // DOES increment the token clock (...and other nodes are locked for writing if their clocks
    // are not equal to authority clock).
    if (isMasterAuthority ())
    {
	assert (urp->getState () == PENDING);
	try
	{
	    DBModule::GetSQLite()->beginTransaction (true);  // exclusive
	    // batch execution, without precompilation!
	    DBModule::GetSQLite()->execUpdate (urp->getSQL ());
	    urp->setState (ACKNOWLEDGED);
	    UpdateRecordTableSync::save (urp.get ());
	    DBModule::GetSQLite()->commitTransaction (); 
	    Log::GetInstance ().info ("Executed : " + Conversion::ToTruncatedString (urp->getSQL ()));
	}
	catch (std::exception& e)
	{
	    Log::GetInstance ().error ("Error while executing SQL statement on master authority : " + 
				       Conversion::ToTruncatedString (urp->getSQL ()), e);
	    urp->setState (FAILED);
	    UpdateRecordTableSync::save (urp.get ());
	}

    }
    else if (urp->getState () == ACKNOWLEDGED)
    {
	UpdateRecordSPtr aur (urp);
	try
	{
	    // The SQL might have been removed if the UpdateRecord was already pending.
	    if (urp->getSQL () == "")
	    {
		UpdateRecordSPtr alreadyIn (_updateLog->getUpdateRecord (urp->getKey ()));
		alreadyIn->setState (ACKNOWLEDGED);
		aur = alreadyIn;
	    }

	    DBModule::GetSQLite()->beginTransaction (true);  // exclusive
	    DBModule::GetSQLite()->execUpdate (aur->getSQL ());
	    UpdateRecordTableSync::save (aur.get ());
	    DBModule::GetSQLite()->commitTransaction (); 
	    Log::GetInstance ().info ("Executed : " + Conversion::ToTruncatedString (aur->getSQL ()));
	}
	catch (std::exception& e)
	{
	    Log::GetInstance ().error ("Error while propagating SQL statement " + 
				       Conversion::ToTruncatedString (aur->getSQL ()), e);
	    aur->setState (FAILED);
	    UpdateRecordTableSync::save (aur.get ());
	}
    }
    else 
    {
	// New PENDING on a non master authority node.
	UpdateRecordTableSync::save (urp.get ());
    }


    for (RingNodes::iterator it = _ringNodes.begin ();
	 it != _ringNodes.end (); ++it)
    {
	RingNodeSPtr rn = it->second;
	
	if (urp->getTimestamp () > rn->getLastPendingTimestamp ())
	{
	    if (urp->getState () == PENDING)
	    {
		rn->setLastPendingTimestamp (urp->getTimestamp ());
	    } 
	    else if (urp->getState () == ACKNOWLEDGED)
	    {
		rn->setLastAcknowledgedTimestamp (urp->getTimestamp ());
	    }
	}
    }
}

    


void 
Node::finalize ()
{

}



    
bool 
Node::canWrite () const
{
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    for (RingNodes::const_iterator it = _ringNodes.begin ();
	 it != _ringNodes.end (); ++it)
    {
	if (it->second->canWrite () == false) return false;
    }
    return true;
}




void Node::dump ()
{
    // for debug
    boost::recursive_mutex::scoped_lock ringNodesLock (*_ringNodesMutex);
    for (RingNodes::iterator it = _ringNodes.begin ();
	 it != _ringNodes.end (); ++it)
    {
	it->second->dump ();
    }
    if (isMasterAuthority ()) std::cerr << "MASTER!! " << std::endl;

}

	

uid 
Node::encodeUpdateKey (NodeId nodeId, long updateId)
{
    // nodeId 12 bits
    // updateId 52  bits
    uid id (nodeId);
    uid tmp = updateId;
    id |= (tmp << 52);
    return id;	    
}




}

}


