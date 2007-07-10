#include "03_db_ring/Node.h"
#include "03_db_ring/DbRingException.h"

#include "03_db_ring/UpdateRecordTableSync.h"
#include "03_db_ring/UpdateChronologyException.h"

#include "02_db/DBModule.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"
  
#include "01_util/Log.h"
#include "01_util/Conversion.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/TcpService.h"
#include "00_tcp/Constants.h"

#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace synthese::tcp;
using namespace synthese::util;
using namespace synthese::db;

using namespace boost::posix_time;


namespace synthese
{
namespace dbring
{




Node::Node (const NodeId& id)
    : _id (id)
    , _ringNodes ()
    , _tcpServices ()
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
	if (_tcpServices.find (nodeInfo.getPort ()) == _tcpServices.end ())
	    _tcpServices.insert (std::make_pair (nodeInfo.getPort (), TcpService::openService (nodeInfo.getPort ())));
    
	TcpService* tcpService = _tcpServices.find (nodeInfo.getPort ())->second;
	
	// Sets this node identity for a given ring.
	_ringNodes.insert (std::make_pair (nodeInfo.getRingId (), RingNodeSPtr (new RingNode (nodeInfo, _updateLog, tcpService))));
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

}
















void 
Node::initialize ()
{
    _lastUpdateIndex = UpdateRecordTableSync::getLastUpdateIndex (_id);
}



bool 
Node::earlyCompileUpdate (const std::string& sql)
{
    bool res (true);

    // Compilation will fail on pure syntactic error, but *also* when a given table does not
    // exist. 
    // select * from toto ;                            => will fail if table toto does not exist in db
    // create table toto (key); select * from toto ;   => OK ! 

    // It can still happen that the statement does not fail locally and fail when executed
    // on authority (race for two updates). But in this case, authority will reject one of the
    // update statements and mark it as failed.
    try
    {
	DBModule::GetSQLite()->finalizeStatement (DBModule::GetSQLite()->prepareStatement (sql));
	return true;
    }
    catch (SQLiteException sqle)
    {
	return false;
    }
}





void 
Node::execUpdate (const std::string& sql)
{
    // Ensure that the update log cannot be modified both by main loop and by local
    // update action. This guarantees that the local update will not occur after token sending
    // and before update log flushing.
    boost::recursive_mutex::scoped_lock updateLogLock (*_updateLogMutex);

    if (canWrite () == false)
    {
	throw DbRingException ("Node is locked for writing");
    }

    // If this is not an update statement, return directly
    if (SQLite::IsUpdateStatement (sql) == false)
    {
	Log::GetInstance ().warn ("Not an update statement, discarded : " + sql);
	return;
    }

    // Early compilation of SQL statement. If the node is writeable, it must be up to date with its authority.
    // It means that it must be in a proper state for execution of the desired SQL code.
    // For this reason, we do an early compilation of the SQL code, and do not propagate anything if
    // the compilation fails.
    if (earlyCompileUpdate (sql) == false)
    {
	Log::GetInstance ().warn ("Early compilation of \"" + sql + "\" failed. Not propagated.");
	return;
    }

    // Use UTC time
    boost::posix_time::ptime now (boost::date_time::microsec_clock<ptime>::universal_time ());

    // Increment record update.
    ++_lastUpdateIndex;
    UpdateRecordSPtr urp (new UpdateRecord (encodeUpdateKey (_id, _lastUpdateIndex) , now,
					    _id, PENDING,
					    sql ));
    saveUpdateRecord (urp);
    

}





void 
Node::loop ()
{
    boost::recursive_mutex::scoped_lock updateLogLock (*_updateLogMutex);

    TokenSPtr token;

    // try to receive token on each open port and dispatch to 
    // corrsponding ring node.
    for (std::map<int, TcpService*>::iterator its = _tcpServices.begin ();
	 its != _tcpServices.end (); ++its)
    {
	TcpService* tcpService = its->second;
	
	token.reset ((Token*) 0);

	synthese::tcp::TcpServerSocket* serverSocket =
	    tcpService->acceptConnection ();
	
	// Non-blocking mode (default)
	if (serverSocket != 0) 
	{
	    token.reset (new Token ());
	    
	    boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
		tcpStream (*serverSocket);
	    
	    char buffer[1024*512]; // 512K buffer max
	    tcpStream.getline (buffer, 1024*512, ETB);
	    
	    // Create token object from received message
	    std::stringstream ss (buffer);
	    try
	    {
		ss >> (*(token.get ()));
	    }
	    catch (std::exception& e)
	    {
		Log::GetInstance ().error ("Error parsing token message", e);
		continue;
	    }

	    // Token successfully parsed
	    // TODO : check cheksum
	    
	    // Acknowledge it
	    tcpStream << ACK;
	    tcpStream.flush();

	    tcpService->closeConnection (serverSocket);

	}

	if (token.get ())
	{
	    // Time to filter update log
	    // filterUpdateLog (token);

	    std::stringstream logstr;
	    logstr << "Node " << _id << " recv [" 
		   << (*token) << "] fm node " << token->getEmitterNodeId () << std::endl;
	    
	    Log::GetInstance ().debug (logstr.str ());
	    
	    mergeUpdateLog (token);
	}
	
	//bool masterAuthority (true);

	for (RingNodes::iterator it = _ringNodes.begin ();
	     it != _ringNodes.end (); ++it)
	{
	    RingNodeSPtr rn = it->second;
	    rn->loop (token);
	}

	

	// At this stage we are sure that local updates have also been taken into account.
	//flushUpdates ();
	
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
    for (UpdateRecordSet::const_iterator itr = records.begin ();
	 itr != records.end (); ++itr)
    {
	UpdateRecordSPtr ur = *itr;

	// Do not propagate failed update record.
	if (ur->getState () == FAILED) continue;

	std::stringstream ss;
	ss << "Merging record " << ur;
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
	    DBModule::GetSQLite()->execUpdate (urp->getSQL ());
	    urp->setState (ACKNOWLEDGED);
	    UpdateRecordTableSync::save (urp.get ());
	    DBModule::GetSQLite()->commitTransaction (); 
	    Log::GetInstance ().info ("Executed : " + urp->getSQL ());
	}
	catch (std::exception e)
	{
	    Log::GetInstance ().error ("Error while executing SQL statement on master authority : " + urp->getSQL ());
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
	    Log::GetInstance ().info ("Executed : " + aur->getSQL ());
	}
	catch (std::exception e)
	{
	    Log::GetInstance ().error ("Error while propagating SQL statement " + aur->getSQL ());
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


