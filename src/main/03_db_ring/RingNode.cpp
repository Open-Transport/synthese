#include "03_db_ring/RingNode.h"
#include "03_db_ring/Constants.h"

#include "03_db_ring/DbRingException.h"
#include "03_db_ring/UpdateRecordTableSync.h"

#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "01_util/Log.h"
#include "01_util/Conversion.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/TcpService.h"
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
		    UpdateLogSPtr& updateLog,
                    synthese::tcp::TcpService* tcpService
		    
    )
    : _data (new Token (nodeInfo.getNodeId (), nodeInfo.getRingId (), updateLog))
    , _timer (min_date_time)
    , _tcpService (tcpService)
{
    setInfo (nodeInfo.getNodeId (), nodeInfo);
}


    
RingNode::~RingNode ()
{
}



void 
RingNode::initialize ()
{
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






bool
RingNode::sendToken ()
{
    // Note that sent token is *exactly* _data held by this node, ie
    // _data which is known by this node at t time and persisted in db,
    // each time it is necessary.
    // There should never be any token copy created.

    if (_data->isModified ())
	_data->setClock (_data->getInfo ().getClock () + 1);

    
    std::vector<NodeId> tryNodes = _data->getNodesAfter (_data->getEmitterNodeId ());

    for (int i=1; i<tryNodes.size (); ++i)
    {
	const NodeInfo& ni = _data->getInfo (tryNodes[i]);

	// Prepare update log for token recipient.
	_data->getUpdateLog ()->flush ();
	
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
	    
	if (sendSurefireToken (ni.getHost (), ni.getPort ())) 
	{
/*
	    std::stringstream logstr;
	    logstr << "Node " << _data->getEmitterNodeId () << " sent [" 
		   << (*_data) << "] to node " << ni.getNodeId () << std::endl;
	    
	    Log::GetInstance ().debug (logstr.str ());
*/
	    return true;
	}


	// Failed to send token, mark node as OUTRING
	_data->setState (ni.getNodeId (), OUTRING);
	
    }

    return false;
    
}



void 
RingNode::loop (const TokenSPtr& token)
{
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

	// ... do some stuff ...
	
	// Forward the token only if is more recent 
	// (or as recent if the token has not been modified), otherwise discard
	NodeInfo tokenInfo (_token->getInfo ());
	NodeInfo localInfo (_data->getInfo ());

	if (localInfo.isAuthority () && (tokenInfo.getClock () < localInfo.getClock ())) 
	{
	    // I am the authority. any deprecated node compared to my local clock is discarded.
            // std::cerr << " ** 00 Discarded" << std::endl;
	} 
	else if ( (tokenInfo.getState () == ENTRING) && 
		  (tokenInfo.isAuthority () == false) &&
		  (_token->getAuthorityState () == INSRING) )
	{
	    // The node is ENTRING but the authority is already INSRING; 
	    // This allows creation of a second token in case the auhtority
	    // is out ring. This way, write-locked nodes can still exchange info between each
	    // other.
	    // std::cerr << " ** 11 Discarded" << std::endl;
	}
	else
	{
	    // Reset the timer only if the token has not been discarded!
	    resetTimer ();

	    if (sendToken () == false)
	    {
		// failed to forward token. revert this node state
		// to OUTRING.
		_data->setState (OUTRING);
	    }
	}

    }
    else if (timedOut ())
    {
	
	// Reset timer.
	resetTimer ();
	// std::cerr << "## TIMEOUT!! " << std::endl;

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


bool 
RingNode::sendSurefireToken (const std::string& host, int port)
{
    bool success (false);

    TcpClientSocket clientSock (host, port, TCP_TOKEN_TIMEOUT);
    for (int i=0; i<SEND_TOKEN_MAX_NB_TRIES; ++i)
    {
	clientSock.tryToConnect ();
	if (clientSock.isConnected () == false) continue;

	try
	{
	    boost::iostreams::filtering_ostream zlibout;

	    boost::iostreams::stream<TcpClientSocket> cliSocketStream;
	    cliSocketStream.open (clientSock);
	    zlibout.push (boost::iostreams::zlib_compressor (boost::iostreams::zlib::best_speed));
	    zlibout.push (cliSocketStream);
	    zlibout << (*_data) << std::flush;
	    zlibout.reset (); // necessary

	    cliSocketStream.close ();
	    success = true;
	    break;

	}
	catch (std::exception& e) 
	{
	    Log::GetInstance ().error ("Error sending token", e);
	}
    }

    return success;
}



}
}


