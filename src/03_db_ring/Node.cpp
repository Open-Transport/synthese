#include "03_db_ring/Node.h"
#include "03_db_ring/Constants.h"
#include "03_db_ring/DbRingException.h"
#include "03_db_ring/NodeInfoTableSync.h"
#include "03_db_ring/ApplyUpdateThreadExec.h"
#include "03_db_ring/UpdateRecordTableSync.h"
#include "03_db_ring/UpdateChronologyException.h"

#include "DBModule.h"
#include "SQLiteException.h"
  
#include "01_util/Log.h"
#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"
#include "01_util/threads/ManagedThread.h"

#include "00_tcp/TcpService.h"
#include "00_tcp/Constants.h"
#include "00_tcp/SocketException.h"


#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace synthese::util;
using namespace synthese::db;
using namespace synthese::tcp;

using namespace boost::posix_time;

extern int nbres;


namespace synthese
{
namespace dbring
{




Node::Node (const NodeId& id, bool isAuthority)
    : _id (id)
    , _isAuthority (isAuthority)
    , _ringNodes ()
    , _lastUpdateIndex (0)
    , _lastAcknowledgedTimestamp (min_date_time)
    , _ringNodesMutex (new boost::recursive_mutex ())
    , _lastAcknowledgedTimestampMutex (new boost::recursive_mutex ())
    , _pendingUpdateRecordsMutex (new boost::recursive_mutex ())
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






void 
Node::setNodeInfoCallback (const NodeInfo& nodeInfo)
{
    boost::recursive_mutex::scoped_lock lock (*_ringNodesMutex);

    RingNodes::iterator it = _ringNodes.find (nodeInfo.getRingId ());
    if (it == _ringNodes.end ()) 
    {
	if (nodeInfo.getNodeId () != _id)
	{
	    
	    throw DbRingException ("Node " + Conversion::ToString (_id) + " is not identified on ring " + Conversion::ToString (nodeInfo.getRingId ()));
	}
	
	Log::GetInstance ().info ("Node " + Conversion::ToString (_id) + " is now identified on ring " + Conversion::ToString (nodeInfo.getRingId ())
				  + " as " + nodeInfo.getHost () + ":" + Conversion::ToString (nodeInfo.getPort ()));
	
	// Sets this node identity for a given ring.
	RingNode* ringNode = new RingNode (nodeInfo);
	_ringNodes.insert (std::make_pair (nodeInfo.getRingId (), RingNodeSPtr (ringNode)));

	ManagedThread* ringNodeThread = 
	    new ManagedThread (ringNode, "dbring_node_" + Conversion::ToString (nodeInfo.getRingId ()), 100);
    }
    else
    {
	it->second->setInfo (nodeInfo);

	// TODO : change tcp service
    }

}




void 
Node::setUpdateRecordCallback (const UpdateRecordSPtr& updateRecord)
{
    // Update timestamps
    if ((updateRecord->getState () != PENDING) && 
	(updateRecord->getTimestamp () > getLastAcknowledgedTimestamp ()))
    {
	boost::recursive_mutex::scoped_lock lock (*_lastAcknowledgedTimestampMutex);
	_lastAcknowledgedTimestamp = updateRecord->getTimestamp ();
    }

    {
	boost::recursive_mutex::scoped_lock lock (*_pendingUpdateRecordsMutex);
	// Update pending records
	if (updateRecord->getState () == PENDING)
	{
	    // std::cerr << "?? Ading " << updateRecord->getKey () << " to pendig ids" << std::endl;
	    _pendingUpdateRecords.insert (updateRecord->getKey ());
	}
	else
	{
	    // std::cerr << "?? Rming " << updateRecord->getKey () << " fm pendig ids" << std::endl;
	    std::set<uid>::iterator it = _pendingUpdateRecords.find (updateRecord->getKey ());
	    if (it != _pendingUpdateRecords.end ())
	    {
		_pendingUpdateRecords.erase (it);
	    }
	}
    }

}





void 
Node::initialize ()
{
    _lastUpdateIndex = UpdateRecordTableSync::GetLastUpdateIndex (_id);

    
    if (_ringNodes.empty ())
    {
	Log::GetInstance ().warn ("This node is not connected to any ring; it will be writable by default.");
    }


    if (isAuthority ())
    {
	Log::GetInstance ().info ("This node is authority.");

	// Launch special authority thread
	ManagedThread* authorityThread = 
	    new ManagedThread (new ApplyUpdateThreadExec (), "dbring_auth", 100); 
    }

}






boost::posix_time::ptime
Node::getLastAcknowledgedTimestamp () const
{
    boost::recursive_mutex::scoped_lock lock (*_lastAcknowledgedTimestampMutex);
    return _lastAcknowledgedTimestamp;
}





synthese::db::SQLiteStatementSPtr 
Node::compileStatement(const synthese::db::SQLData& sql)
{
    return DBModule::GetSQLite()->compileStatement (sql);
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
Node::execUpdate (const SQLiteStatementSPtr& statement)
{
    execUpdate (statement->getSQL ());
}






void 
Node::execUpdate (const SQLData& sql)
{
    // If this is not an update statement, return directly
    if (SQLite::IsUpdateStatement (sql) == false)
    {
	Log::GetInstance ().warn ("Not an update statement, discarded : " + sql);
	return;
    }

    // Use UTC time
    boost::posix_time::ptime now (boost::date_time::microsec_clock<ptime>::universal_time ());

    // Compress sql
    std::stringstream decompressedSQL (sql);
    std::stringstream compressedSQL;
    Compression::ZlibCompress (decompressedSQL, compressedSQL);
    
    // Increment record update.
    ++_lastUpdateIndex;
    UpdateRecordSPtr urp (new UpdateRecord (encodeUpdateKey (_id, _lastUpdateIndex) , now,
					    _id, PENDING, compressedSQL.str ()));

    
    {
	boost::recursive_mutex::scoped_lock lock (*_pendingUpdateRecordsMutex);
	_pendingUpdateRecords.insert (urp->getKey ());
    }

    saveUpdateRecord (urp);

    // Wait for this update record to be acknowledged
    while (true)
    {
	{
	    boost::recursive_mutex::scoped_lock lock (*_pendingUpdateRecordsMutex);
	    if (_pendingUpdateRecords.find (urp->getKey ()) == _pendingUpdateRecords.end ()) break;
	}
    }
}





void 
Node::loop ()
{
    static const int maxlen (1024); 
    static char buf[maxlen];
	    
    boost::recursive_mutex::scoped_lock lock (*_ringNodesMutex);

    // If several ring nodes use the same service port, this loop 
    // ensures that the port will accept connection several times as well.
    for (RingNodes::iterator it = _ringNodes.begin ();
	 it != _ringNodes.end (); ++it)
    {
	RingNodeSPtr rn = it->second;

	int backlogSize = 1;
	TcpService* tcpService = TcpService::openService (rn->getInfo ().getPort (), true, true, backlogSize);
	
	synthese::tcp::TcpServerSocket* serverSocket =
	    tcpService->acceptConnection ();

	// Non-blocking mode (default)
	if (serverSocket != 0) 
	{
	    int serverTimeout = 200;
	    serverSocket->setTimeOut (serverTimeout);

	    try
	    {

		// First get client node info
		std::stringstream ss;
		char buf[1];
		while (true)
		{
		    serverSocket->read (buf, 1);
		    if (buf[0] == ETB) break;
		    ss << buf[0];
		}
		
		NodeInfo clientNodeInfo;
		ss >> clientNodeInfo;
		
		RingNodeSPtr ringNode = _ringNodes.find (clientNodeInfo.getRingId ())->second;

		ringNode->serverLoop (clientNodeInfo, rn->getInfo ().getPort (), serverSocket);
	    } 
	    catch (SocketException& se)
	    {
		// Silently ignore timeout.
	    }
	}

    }
    
}




bool 
Node::isAuthority () const
{
    return _isAuthority;
}





void 
Node::saveNodeInfo (NodeInfo info)
{
    NodeInfoTableSync::save ((NodeInfo*) &info);
    
    // This will trigger the hook and the memory state will be acknowledged through
    // the db callback (with setNodeInfoCallback). 
}
 



void
Node::saveUpdateRecord (const UpdateRecordSPtr& urp)
{

    if (isAuthority ())
    {
	if (urp->getState () == PENDING)
	{
	    UpdateRecordTableSync::save (urp.get ());
	}
    }
    else 
    {
	// Locally pending record.
	if (urp->getState () == PENDING)
	{
	    UpdateRecordTableSync::save (urp.get ());
	}

	// These are notifications of record ACKNOWLEDGED or FAILED by authority.
	// We must apply them as well locally.
	// The difference is that we do not overwrite timestamp so as to stay consistent
	// with authority chronology.
	else if (urp->getState () == ACKNOWLEDGED)
	{
	    // Does it hold compressed SQL ? If yes, it means that this record was
	    // not even pending on this node. So save it first as a pending record.
	    if (urp->hasCompressedSQL ())
	    {
		// If we receive an apply update order with sql we must first save it as PENDING
		urp->setState (PENDING);
		UpdateRecordTableSync::save (urp.get ());
	    }

	    UpdateRecordTableSync::ApplyUpdateRecord (urp, false);
	}
	else if (urp->getState () == FAILED)
	{
	    UpdateRecordTableSync::save (urp.get ());

	    // It will be saved, not matter if it was before in db because
	    // we do not care about failed SQL.
	    UpdateRecordTableSync::AbortUpdateRecord (urp, false);
	}
    }

}


    


void 
Node::finalize ()
{

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




std::set<uid> 
Node::getPendingUpdateRecords () const
{
    // returns a copy for thread safety.
    boost::recursive_mutex::scoped_lock lock (*_pendingUpdateRecordsMutex);
    return _pendingUpdateRecords;
}






}

}


