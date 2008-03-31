
/** NodeClientThreadExec class implementation.
	@file NodeClientThreadExec.cpp

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

#include "03_db_ring/NodeClientThreadExec.h"
#include "03_db_ring/Constants.h"
#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/UpdateRecordTableSync.h"
#include "03_db_ring/Node.h"

#include "01_util/threads/Thread.h"
#include "01_util/Log.h"
#include "01_util/iostreams/Compression.h"

#include "00_tcp/Constants.h"
#include "00_tcp/SocketException.h"

#include <boost/iostreams/stream.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


using namespace boost::posix_time;


using namespace synthese::tcp;
using namespace synthese::util;



namespace synthese
{
    namespace dbring
    {

	NodeClientThreadExec::NodeClientThreadExec (
	    NodeInfo serverNodeInfo,
	    NodeInfo clientNodeInfo,
	    NodeInfoMap clientNodeInfoMap,
	    TransmissionStatusMap& clientStatusMap)
	    
	    : ThreadExec ()
	    , _serverNodeInfo (serverNodeInfo)
	    , _clientNodeInfo (clientNodeInfo)
	    , _clientNodeInfoMap (clientNodeInfoMap)
	    , _clientStatusMap (clientStatusMap)
	{
	}
	

	NodeClientThreadExec::~NodeClientThreadExec ()
	{
	}


	void 
	NodeClientThreadExec::initialize ()
	{
	}




	void 
	NodeClientThreadExec::finalize ()
	{
	}



	void 
	NodeClientThreadExec::loop ()
	{
	    NodeId serverNodeId = _serverNodeInfo.getNodeId ();

	    _clientStatusMap.setTransmissionStatus (serverNodeId, INPROGRESS);

	    UpdateRecordSet receivedUpdateSet;
	    char buf[512];
	    bool failed (false);
	    try 
	    {
		// We set a very long timeout here.
		// We could set infinite timeout (0).
		// But this way, if server node changes its port of comm, this gives a chance to 
		// interrupt this thread.
		int clientTimeout = 60*1000; // 1 minute

		TcpClientSocket clientSocket (_serverNodeInfo.getHost (), _serverNodeInfo.getPort () , clientTimeout);
		clientSocket.tryToConnect ();
	
		if (clientSocket.isConnected () == false) 
		{
		    _clientStatusMap.setTransmissionStatus (serverNodeId, FAILURE);
		    return;
		}
	
		boost::iostreams::stream<TcpClientSocket> tcpStream (clientSocket);

		if (!tcpStream.good ()) throw 1;
		
		boost::posix_time::ptime lastAcknowledgedTimestamp = DbRingModule::GetNode ()->getLastAcknowledgedTimestamp ();
		tcpStream << _clientNodeInfo << ETB << to_iso_string (lastAcknowledgedTimestamp) << ETB << std::flush;
		
		if (!tcpStream.good ()) throw 1;

		// Now send a space separated list (compressed) 
		// of all pending update records uid after lastAcknowledgedTimestamp
		std::stringstream pss;
		std::set<uid> pur = DbRingModule::GetNode ()->getPendingUpdateRecords ();
		// UpdateRecordTableSync::LoadPendingRecordIds (pur);

		for (std::set<uid>::const_iterator it = pur.begin ();
		     it != pur.end (); ++it)
		{
		    if (it != pur.begin ()) pss << " ";
		    pss << *it;
		}

		Compression::ZlibCompress (pss, tcpStream);
		tcpStream.flush ();

		if (!tcpStream.good ()) throw 1;

		// Now wait for a list of node infos.
		tcpStream.getline (buf, sizeof(buf), ETB);

		if (!tcpStream.good ()) throw 1;

		int nbNodeInfos (Conversion::ToInt (std::string (buf)));

		for (int i=0; i<nbNodeInfos; ++i)
		{
		    tcpStream.getline (buf, sizeof(buf), ETB);
		    if (!tcpStream.good ()) throw 1;
		    std::stringstream tmp;
		    tmp << std::string (buf);
		    NodeInfo nodeInfo;
		    tmp >> nodeInfo;
		    NodeInfoMap::const_iterator itni = _clientNodeInfoMap.find (nodeInfo.getNodeId ());
		    if ( (itni == _clientNodeInfoMap.end ()) ||
			 (itni->second != nodeInfo) )
		    {
			DbRingModule::GetNode ()->saveNodeInfo (nodeInfo);
		    } 
		}

		// Now wait for corresponding update log
		
		tcpStream >> receivedUpdateSet ;
		if (!tcpStream.good ()) throw 1;

		if (receivedUpdateSet.size () > 0)
		{
		    Log::GetInstance ().info ("Received " + Conversion::ToString (receivedUpdateSet.size ()) + " records since "
					      + to_iso_string (lastAcknowledgedTimestamp));
		}

		// Acknowledge the last update record
		tcpStream << ACK << std::flush;
		if (!tcpStream.good ()) throw 1;

		
		tcpStream.close ();
	    }
	    catch (tcp::SocketException& soe) 
	    {
		// silently catched...
		failed = true;
	    }
	    catch (std::exception& e) 
	    {
		Log::GetInstance ().error ("Error during node client loop", e);
		failed = true;
	    }
	    catch (...) 
	    {
		// silently catched...
		failed = true;
	    }

	    // If transmission failed, do not save any records received.
	    if (failed == false)
	    {
		try
		{
		    // Merge it
		    for (UpdateRecordSet::const_iterator itr = receivedUpdateSet.begin ();
			 itr != receivedUpdateSet.end (); ++itr)
		    {
			UpdateRecordSPtr ur = *itr;
			DbRingModule::GetNode ()->saveUpdateRecord (ur);
		    }
		    
		}
		catch (std::exception& e) 
		{
		    Log::GetInstance ().error ("Error propagating update record ", e);
		}
	    }
	    
	    _clientStatusMap.setTransmissionStatus (serverNodeId, READY);
	    
	}
	
    }    
}

