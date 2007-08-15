#include "03_db_ring/Token.h"

#include "03_db_ring/DbRingException.h"
#include "03_db_ring/NodeInfoTableSync.h"
#include "03_db_ring/UpdateRecordTableSync.h"

#include "01_util/Log.h"

#include "00_tcp/Constants.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>

#include <boost/iostreams/copy.hpp>

#include <sstream>
#include <assert.h>


using namespace synthese::tcp;
using namespace synthese::util;
using namespace boost::posix_time;


namespace synthese
{

namespace dbring
{


Token::Token ()
    : _emitterNodeId (-1)
    , _emitterRingId (-1)
    , _authorityNodeId (-1)
    , _infos ()
    , _modified (false)
    , _updateLog (new UpdateLog ())
    , _infosMutex (new boost::recursive_mutex ())
    , _modifiedMutex (new boost::recursive_mutex ())
    , _authorityMutex (new boost::recursive_mutex ())
{

}




Token::Token (const NodeId& emitterNodeId, 
	      const RingId& emitterRingId,
	      UpdateLogSPtr& updateLog
    )
    : _emitterNodeId (emitterNodeId)
    , _emitterRingId (emitterRingId)
    , _authorityNodeId (-1)
    , _infos ()
    , _modified (false)
    , _updateLog (updateLog)
    , _infosMutex (new boost::recursive_mutex ())
    , _modifiedMutex (new boost::recursive_mutex ())
    , _authorityMutex (new boost::recursive_mutex ())
{
    
}



Token::~Token ()
{

}

    



NodeId
Token::getEmitterNodeId () const
{
    return _emitterNodeId;
}



NodeId 
Token::getAuthorityNodeId () const
{
    boost::recursive_mutex::scoped_lock infosLock (*_authorityMutex);
    return _authorityNodeId;
}


NodeId
Token::getEmitterRingId () const
{
    return _emitterRingId;
}



bool 
Token::hasRecipient () const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);

    return _infos.size () > 1;
}


bool 
Token::hasInfo (const NodeId& nodeId) const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);

    NodeInfoMap::const_iterator it = _infos.find (nodeId);
    return (it != _infos.end ());
}



NodeInfo
Token::getInfo (const NodeId& nodeId) const
{
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);
    assert (hasInfo (nodeId));
    return _infos.find (nodeId)->second;
}




NodeInfo
Token::getInfo () const
{
    return getInfo (_emitterNodeId);
}


NodeState 
Token::getAuthorityState () const
{
    boost::recursive_mutex::scoped_lock infosLock (*_authorityMutex);
    // if the authority is unknown, consider the authority as if it was
    // OUTRING.
    if (_authorityNodeId == -1) return OUTRING;
    return getInfo (_authorityNodeId).getState ();
}







void 
Token::setInfo (const NodeId& nodeId, const NodeInfo& info)
{
    boost::recursive_mutex::scoped_lock authorityLock (*_authorityMutex);
    boost::recursive_mutex::scoped_lock infosLock (*_infosMutex);

    _infos[nodeId] = info;

    if (info.isAuthority ()) 
    {
	if ((_authorityNodeId != -1) && (_authorityNodeId != nodeId))
	{
	    throw DbRingException ("An authority is already defined !");
	}
	else
	{
	    _authorityNodeId = nodeId;
	}
    }
    else if (nodeId == _authorityNodeId)
    {
	_authorityNodeId = -1;
    }
}







void 
Token::saveInfo (const NodeId& nodeId,
		 NodeInfo info)
{

    // Compare all infos but clock
    if (hasInfo (nodeId) && (getInfo (nodeId) == info) )
    {
	// if clock is equal as well, then really nothing to do.
	if (getInfo (nodeId).getClock () == info.getClock ()) 
	{
	    return; 
	}
    }
    else
    {
	// An important info (other than clock) is updated.
	// We can mark this token as modified.
	setModified (true);
    }

    // Reporting 

    if (hasInfo (nodeId) == false)
    {
	Log::GetInstance ().info ("Node " + Conversion::ToString (nodeId) + " joined ring " +
				  Conversion::ToString (info.getRingId ()) + " as " +
				  info.getHost () + ":" +  Conversion::ToString (info.getPort ()) );
    }
    else
    {
	NodeInfo oldInfo = getInfo (nodeId);
	if ((oldInfo.getHost () != info.getHost ()) ||
	    (oldInfo.getPort () != info.getPort ()))
	{
	    Log::GetInstance ().info ("Node " + Conversion::ToString (nodeId) + " has changed its identity on ring " +
					 Conversion::ToString (info.getRingId ()) + " from " + oldInfo.getHost () + ":" 
				      + Conversion::ToString (oldInfo.getPort ()) + 
				      " to " + info.getHost () + ":" +  Conversion::ToString (info.getPort ()));
	}
	
	if (oldInfo.isAuthority () != info.isAuthority ())
	{
	    // Cannot happen right now.
	}

	if (oldInfo.getState () != info.getState ())
	{
	    if ((oldInfo.getState () == OUTRING) && (info.getState () == ENTRING))
		Log::GetInstance ().info ("Node " + Conversion::ToString (nodeId) + " is entering ring " + Conversion::ToString (info.getRingId ()));

	    if ((oldInfo.getState () == ENTRING) && (info.getState () == INSRING))
		Log::GetInstance ().info ("Node " + Conversion::ToString (nodeId) + " is inside ring " + Conversion::ToString (info.getRingId ()));

	    if (info.getState () == OUTRING)
		Log::GetInstance ().info ("Node " + Conversion::ToString (nodeId) + " has just left ring " + Conversion::ToString (info.getRingId ()));
	}

	

    }


    // Persist this state in db; it is important
    // that the in memory model is not updated before the new state is persisted in DB.
    // (in case of system crash). This is why info parameter is passed by value.
    NodeInfoTableSync::save ((NodeInfo*) &info);
    
    // This will trigger the hook and the memory state will be updated through
    // the db callback (with setInfo). 

}
 

void 
Token::setState (const NodeId& nodeId, const NodeState& state)
{
    NodeInfo newInfo (getInfo (nodeId));
    newInfo.setState (state);
    
    saveInfo (nodeId, newInfo); 
}



boost::posix_time::ptime
Token::getLastPendingTimestamp (const NodeId& nodeId) const
{
    return getInfo (nodeId).getLastPendingTimestamp ();
}



boost::posix_time::ptime
Token::getLastPendingTimestamp () const 
{
    return getLastPendingTimestamp (_emitterNodeId);
}




void 
Token::setLastPendingTimestamp (const NodeId& nodeId, const boost::posix_time::ptime& lastPendingTimestamp)
{
    NodeInfo newInfo (getInfo (nodeId));
    newInfo.setLastPendingTimestamp (lastPendingTimestamp);

    saveInfo (nodeId, newInfo); 
}




void 
Token::setLastPendingTimestamp (const boost::posix_time::ptime& lastPendingTimestamp)
{
    setLastPendingTimestamp (_emitterNodeId, lastPendingTimestamp);
}






boost::posix_time::ptime
Token::getLastAcknowledgedTimestamp (const NodeId& nodeId) const
{
    return getInfo (nodeId).getLastAcknowledgedTimestamp ();
}



boost::posix_time::ptime
Token::getLastAcknowledgedTimestamp () const 
{
    return getLastAcknowledgedTimestamp (_emitterNodeId);
}




void 
Token::setLastAcknowledgedTimestamp (const NodeId& nodeId, const boost::posix_time::ptime& lastAcknowledgedTimestamp)
{
    NodeInfo newInfo (getInfo (nodeId));
    newInfo.setLastAcknowledgedTimestamp (lastAcknowledgedTimestamp);

    saveInfo (nodeId, newInfo); 
}




void 
Token::setLastAcknowledgedTimestamp (const boost::posix_time::ptime& lastAcknowledgedTimestamp)
{
    setLastAcknowledgedTimestamp (_emitterNodeId, lastAcknowledgedTimestamp);
}





void 
Token::setState (const NodeState& state)
{
    setState (_emitterNodeId, state);
}








std::vector<NodeId> 
Token::getNodesAfter (const NodeId& id) const
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
Token::merge (const TokenSPtr& token)
{
    boost::recursive_mutex::scoped_lock infosLock (*(token->_infosMutex));

    assert (token->getEmitterNodeId () != -1);
    assert (token->getEmitterRingId () != -1);

    // merge node infos
    
    for (NodeInfoMap::const_iterator it = token->_infos.begin ();
	 it != token->_infos.end (); ++it)
    {
	if (it->first == _emitterNodeId) 
	{
	    // This node is authoritative on its own state
	    continue;
	}

	const NodeId& nid = it->first;
	const NodeInfo& nif = it->second;

	if ( (token->getInfo ().getClock () >= getInfo ().getClock ()) ||  
	     (hasInfo (token->getEmitterNodeId ()) == false) )  
//	if ( (nif.getClock () >= getInfo (nid).getClock ()) ||  
//	     (hasInfo (nid) == false) )  
	{

	    // merge info only if recevd token has been updated more recently.
	    // than this node.
	    saveInfo (it->first, it->second);
	} 

    }

    if ((token->getInfo ().getClock () > getInfo ().getClock ()))
    {
	setClock (token->getInfo ().getClock ());
    }
    
}


void 
Token::setClock (const TokenClock& clock)
{
    setClock (_emitterNodeId, clock);
}




void 
Token::setClock (const NodeId& nodeId, const TokenClock& clock)
{
    NodeInfo newInfo (getInfo (nodeId));
    newInfo.setClock (clock);

    // Token clock is meant to count token updates, excluding of course clock updates.
    // => do not use saveInfo to prevent _modified flag to be 
    // set when updating clock 
    NodeInfoTableSync::save ((NodeInfo*) &newInfo);
}



bool 
Token::isModified () const
{
    boost::recursive_mutex::scoped_lock modifiedLock (*_modifiedMutex);
    return _modified;
}



void 
Token::setModified (bool modified)
{
    boost::recursive_mutex::scoped_lock modifiedLock (*_modifiedMutex);
    _modified = modified;
}






std::ostream& 
operator<< ( std::ostream& os, const Token& op )
{
    boost::recursive_mutex::scoped_lock infosLock (*op._infosMutex);
    
    os << op._emitterNodeId << " "
       << op._emitterRingId << " " << ETB;

    for (std::map<NodeId, NodeInfo>::const_iterator it = op._infos.begin ();
	 it != op._infos.end (); ++it)
    {
	os << it->second << ETX; 
    }
    os << ETB;

    os << *(op._updateLog) << ETB;
    
    return os;
}



std::istream& 
operator>> ( std::istream& is, Token& op )
{
    boost::recursive_mutex::scoped_lock infosLock (*op._infosMutex);
    static const int maxlen (1024*1024*64); 
    static char buf[maxlen];

    std::stringstream ids;
    std::stringstream nodeInfos;
    std::stringstream updateRecords;
    
    is.getline (buf, maxlen, ETB);
    ids << buf;

    is.getline (buf, maxlen, ETB);
    nodeInfos << buf;

    is.getline (buf, maxlen, ETB);
    updateRecords << buf;

    ids >> op._emitterNodeId;
    ids >> op._emitterRingId;

    std::string ts;
    ids >> ts;

    while (nodeInfos.getline (buf, maxlen, ETX))
    {
	std::stringstream input (buf);
	NodeInfo nodeInfo;
	input >> nodeInfo;
	op.setInfo (nodeInfo.getNodeId (), nodeInfo);
    }

    updateRecords >> (*(op._updateLog));
    
    return is;

}


}
}




