
/** RingNode class implementation.
	@file RingNode.cpp

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

#include "00_tcp/TcpClientSocket.h" // WINDOWS BUG : LET IT AT FIRST POSITION

#include "03_db_ring/RingNode.h"
#include "03_db_ring/Constants.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/DbRingException.h"
#include "03_db_ring/UpdateRecordTableSync.h"
#include "03_db_ring/Node.h"

#include "03_db_ring/NodeServerThreadExec.h"
#include "03_db_ring/NodeClientThreadExec.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "01_util/Log.h"
#include "01_util/threads/Thread.h"
#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"

#include "00_tcp/Constants.h"
#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"



using namespace synthese::tcp;
using namespace synthese::util;

using namespace boost::posix_time;


namespace synthese
{
namespace dbring
{


const time_duration RingNode::UNREACHABLE_NODES_RETRY_TIMEOUT = milliseconds (10000);




RingNode::RingNode (const NodeInfo& nodeInfo)
    : _nodeId (nodeInfo.getNodeId ())
    , _ringId (nodeInfo.getRingId ())
    , _unreachableNodesRetryTimer (min_date_time)
    , _infos ()
    , _clientStatusMap ()
    , _serverStatusMap ()
    , _infosMutex (new boost::recursive_mutex ())
{
    setInfo (nodeInfo);
}


    
RingNode::~RingNode ()
{
}



std::vector<NodeId> 
RingNode::getNodesAfter (const NodeId& id) const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);

    std::vector<NodeId> nodesAfter;
    NodeInfoMap::const_iterator it = _infos.find (id);
    assert (it != _infos.end ());

    // append this searched node itself
    nodesAfter.push_back (it->first);

    ++it;
    while (it != _infos.end ())
    {
	nodesAfter.push_back (it->first);
	++it;
    }
    
    it = _infos.begin ();
    while (it != _infos.find (id))
    {
	nodesAfter.push_back (it->first);
	++it;
    }
    return nodesAfter;
}





void 
RingNode::initialize ()
{
}




bool 
RingNode::hasInfo (const NodeId& nodeId) const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);

    NodeInfoMap::const_iterator it = _infos.find (nodeId);
    return (it != _infos.end ());
}


NodeInfo  
RingNode::getInfo (const NodeId& nodeId) const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);
    assert (hasInfo (nodeId));
    return _infos.find (nodeId)->second;
}



NodeInfo  
RingNode::getInfo () const
{
    return getInfo (_nodeId);
}


    
    
void 
RingNode::setInfo (const NodeInfo& info)
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);
    _infos[info.getNodeId ()] = info;
}






void 
RingNode::resetUnreachableNodesRetryTimer ()
{
    _unreachableNodesRetryTimer = boost::date_time::microsec_clock<ptime>::local_time ();
}




bool 
RingNode::unreachableNodesRetryTimedOut () const
{
    ptime checkTime = boost::date_time::microsec_clock<ptime>::local_time ();
    return (checkTime - _unreachableNodesRetryTimer > UNREACHABLE_NODES_RETRY_TIMEOUT);
}   



void
RingNode::loop ()
{

    // Check that unreachable nodes are tried sometimes...
    if (unreachableNodesRetryTimedOut ())
    {
	resetUnreachableNodesRetryTimer ();
	_clientStatusMap.reset (); 
    }

    clientLoop ();

}



void 
RingNode::clientLoop ()
{
    std::vector<NodeId> followers = getNodesAfter (_nodeId);;

    bool allFailed (followers.size () > 0);

    // And immediately, try to initiate a new client connection with one of of followers (if not in transmission)
    for (int n = 1; n<followers.size (); ++n)
    {
	// If last client connection to node has failed, try another one on ring.
	if (_clientStatusMap.getTransmissionStatus (followers[n]) == FAILURE) continue;

	allFailed = false;

	// Do not initiate another connection with a node is another one is in progress.
	if (_clientStatusMap.getTransmissionStatus (followers[n]) != READY)
	{
	    break;
	}
	
	const NodeInfo& tryNode = getInfo (followers[n]);

	Thread::RunOnce (new NodeClientThreadExec (
			     tryNode,
			     getInfo (),
			     _infos,
			     _clientStatusMap));

	break;
    }    

    if (allFailed) 
    {
	resetUnreachableNodesRetryTimer ();
	_clientStatusMap.reset (); 
    }


}





void 
RingNode::serverLoop (NodeInfo clientNodeInfo,  int port, TcpServerSocket* serverSocket)
{
    
    // Merge client node info
    NodeInfoMap::const_iterator itni = _infos.find (clientNodeInfo.getNodeId ());
    if ( (itni == _infos.end ()) ||
	 (itni->second != clientNodeInfo) )
    {
	DbRingModule::GetNode ()->saveNodeInfo (clientNodeInfo);
    } 


    if (_serverStatusMap.getTransmissionStatus (clientNodeInfo.getNodeId ()) != READY)
    {
	TcpService::openService (port)->closeConnection (serverSocket);
	return;
    }

    std::vector<NodeId> followers = getNodesAfter (_nodeId);

    // A channel has been established by the node
    // Do a synchronization cycle with the client ring node.
    // A synchronization cycle is composed of 2 steps :
    // 1) N2 sends N1 its lastPendingTimestamp (pending or acknowledged)
    // 2) N1 sends token to N2 with acknowledged log prepared for the returned timestamp
    // 3) N2 acknowledged N1.
    
    // Spawn immediately a node server thread exec to handle tcp query.
    Thread::RunOnce (new NodeServerThreadExec (clientNodeInfo,
					       port,
					       serverSocket, 
					       _infos,
					       _serverStatusMap));
    

}










}
}


