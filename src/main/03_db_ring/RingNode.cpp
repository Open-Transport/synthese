#include "03_db_ring/RingNode.h"
#include "03_db_ring/Constants.h"

#include "03_db_ring/DbRingException.h"
#include "03_db_ring/UpdateRecordTableSync.h"

#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "01_util/Log.h"
#include "01_util/threads/Thread.h"
#include "01_util/Conversion.h"

#include "00_tcp/Constants.h"


using namespace synthese::tcp;
using namespace synthese::util;

using namespace boost::posix_time;


namespace synthese
{
namespace dbring
{


const time_duration RingNode::RECV_TOKEN_TIMEOUT = milliseconds (4000);
const int RingNode::SEND_TOKEN_MAX_NB_TRIES (3);





RingNode::RingNode (const NodeInfo& nodeInfo,
		    UpdateLogSPtr& updateLog)
    : _data (new Token (nodeInfo.getNodeId (), nodeInfo.getRingId (), updateLog))
    , _timer (min_date_time)
    , _transmissionStatusMap ()
{
    setInfo (nodeInfo.getNodeId (), nodeInfo);
}


    
RingNode::~RingNode ()
{
}



void 
RingNode::initialize ()
{
    // Set initial state of all known nodes to OUTRING
    std::vector<NodeId> nodesAfter = 
	_data->getNodesAfter (getInfo ().getNodeId ());
    for (int n=1; n<nodesAfter.size (); ++n)
    {
	_data->setState (nodesAfter[n], OUTRING);
    }


}




bool 
RingNode::hasInfo (const NodeId& nodeId) const
{
    return _data->hasInfo (nodeId);
}


NodeInfo  
RingNode::getInfo (const NodeId& nodeId) const
{
    return _data->getInfo (nodeId);
}



NodeInfo  
RingNode::getInfo () const
{
    return _data->getInfo ();
}







void 
RingNode::setInfo (const NodeId& nodeId, const NodeInfo& nodeInfo)
{
    _data->setInfo (nodeId, nodeInfo);
}




void 
RingNode::setModified (bool modified)
{
    _data->setModified (modified);
}


    



boost::posix_time::ptime
RingNode::getLastPendingTimestamp () const
{
    return _data->getLastPendingTimestamp ();
}




void 
RingNode::setLastPendingTimestamp (const boost::posix_time::ptime& lastPendingTimestamp)
{
    _data->setLastPendingTimestamp (lastPendingTimestamp);
}


boost::posix_time::ptime
RingNode::getLastAcknowledgedTimestamp () const
{
    return _data->getLastAcknowledgedTimestamp ();
}




void 
RingNode::setLastAcknowledgedTimestamp (const boost::posix_time::ptime& lastAcknowledgedTimestamp)
{
    _data->setLastAcknowledgedTimestamp (lastAcknowledgedTimestamp);
}



bool 
RingNode::canWrite () const
{

    // TODO : check that a node can only be connected by its authority!

    NodeId authorityNodeId = _data->getAuthorityNodeId ();
    return (authorityNodeId == _data->getInfo ().getNodeId ()) ||
	 ((authorityNodeId != -1) &&
	 (getInfo (authorityNodeId).getState () == INSRING) &&
	 (getInfo (authorityNodeId).getClock () == _data->getInfo ().getClock ()));
}





void 
RingNode::resetTimer ()
{
    _timer = boost::date_time::microsec_clock<ptime>::local_time ();
}



bool 
RingNode::timedOut () const
{
    ptime checkTime = boost::date_time::microsec_clock<ptime>::local_time ();
    return (checkTime - _timer > RECV_TOKEN_TIMEOUT);
}   






void
RingNode::sendToken ()
{
    // Note that sent token is *exactly* _data held by this node, ie
    // _data which is known by this node at t time and persisted in db,
    // each time it is necessary.
    // There should never be any token copy created.

    if (_data->isModified ())
	_data->setClock (_data->getInfo ().getClock () + 1);

    
    std::vector<NodeId> tryNodes = _data->getNodesAfter (_data->getEmitterNodeId ());

    
    // Send to all nodes til the first node with unknown state is found.
    for (int i=1; i<tryNodes.size (); ++i)
    {
	const NodeInfo& ni = _data->getInfo (tryNodes[i]);

	TransmissionStatus transmissionStatus = _transmissionStatusMap.getTransmissionStatus (ni.getNodeId ());
	
	// Prepare update log for token next recipient.
	_data->getUpdateLog ()->flush ();

	// If recipient is flagged as INSRING, send a data update
	if (ni.getState () == INSRING)
	{
	    // Temporary logs
	    UpdateLogSPtr sinceLastPending (new UpdateLog ());
	    UpdateLogSPtr sinceLastAcknowledged (new UpdateLog ());
	    
	    UpdateRecordTableSync::loadAllAfterTimestamp (sinceLastAcknowledged, ni.getLastAcknowledgedTimestamp ());
	    for (UpdateRecordSet::iterator it = sinceLastAcknowledged->getUpdateRecords ().begin ();
		 it != sinceLastAcknowledged->getUpdateRecords ().end (); ++it)
	    {
		UpdateRecordSPtr ur = *it;
		if ((ur->getState () == ACKNOWLEDGED) && (ur->getTimestamp () <= ni.getLastPendingTimestamp ()))
		{
		    // SQL for this update record was already transmitted. Remove SQL part and acknowledge it.
		    ur->setSQL ("");
	    }
		_data->getUpdateLog ()->setUpdateRecord (ur);
	    }
	}

	// Token is prepared, spawn the sending thread.
	Thread::RunOnce (new SendTokenThreadExec (_data->getEmitterNodeId (), ni, _data, _transmissionStatusMap));
	
	if (transmissionStatus == UNKNOWN)
	{ 
	    break;
	}
    }

}



void 
RingNode::loop (const TokenSPtr& token)
{
    // Update NodeInfo states according to Transmission statuses.
    std::vector<NodeId> followers = _data->getNodesAfter (_data->getEmitterNodeId ());;
    bool allFailed (followers.size () > 0);


    for (int n = 1; n<followers.size (); ++n)
    {
	TransmissionStatus transmissionStatus = _transmissionStatusMap.getTransmissionStatus (followers[n]);
	if (transmissionStatus == FAILURE)
	{
	    _data->setState (followers[n], OUTRING);
	    continue;
	}
	
	if (transmissionStatus == SUCCESS)
	{
	    _transmissionStatusMap.setTransmissionStatus (followers[n], UNKNOWN);
	}

	allFailed = false;
    }

    if (allFailed) 
    {
	// _data->setState (OUTRING);
	for (int n = 1; n<followers.size (); ++n)
	{
	    _transmissionStatusMap.setTransmissionStatus (followers[n], UNKNOWN);
	}
    }

    if (token && (token->getEmitterRingId () != getInfo ().getRingId ())) return;

    _token =  token;

    if (_data->hasRecipient () == false) 
    {
	// knows only itself...
    }
    
    if (_token.get ())
    {
	// A token was received. 

	// Update local state
	if (_data->getInfo ().getState () == ENTRING)	{
	    _data->setState (INSRING);
	} 
	else if (_data->getInfo ().getState () == OUTRING)
	{
	    _data->setState (ENTRING);
	}

	// Merge token info.
	_data->merge (_token);

	// Forward the token only if is more recent 
	// (or as recent if the token has not been modified), otherwise discard
	NodeInfo tokenInfo (_token->getInfo ());
	NodeInfo localInfo (_data->getInfo ());

	if (localInfo.isAuthority () && (tokenInfo.getClock () < localInfo.getClock ())) 
	{
	    // I am the authority. any deprecated node compared to my local clock is discarded.
            std::cerr << " ** 00 Discarded" << std::endl;
	} 

	else if ( (tokenInfo.getState () == ENTRING) && 
		  (tokenInfo.isAuthority () == false) &&
		  (_token->getAuthorityState () == INSRING) )
	{
	    // The node is ENTRING but the authority is already INSRING; 
	    // This allows creation of a second token in case the auhtority
	    // is out ring. This way, write-locked nodes can still exchange info between each
	    // other.
	    std::cerr << " ** 11 Discarded" << std::endl;
	} 

	else
	{
	    // Reset the timer only if the token has not been discarded!
	    resetTimer ();

	    sendToken ();
	}

    }
    else if (timedOut ())
    {
	
	// Reset timer.
	resetTimer ();
	std::cerr << "## TIMEOUT!! " << std::endl;

	// Create init token...
	_data->setState (ENTRING);

	// ...and send it in any case
	sendToken ();
    }    
    

    // One exit point : here.
    setModified (false);


}


void RingNode::dump ()
{
    // for debug
    std::cerr << (*_data) << " " << (canWrite () ? "WRITEABLE" : "READABLE") << std::endl;

}




}
}


