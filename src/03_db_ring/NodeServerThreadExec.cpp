
/** NodeServerThreadExec class implementation.
	@file NodeServerThreadExec.cpp

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

#include "03_db_ring/NodeServerThreadExec.h"
#include "03_db_ring/Constants.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/UpdateRecordTableSync.h"


#include "01_util/Log.h"
#include "01_util/iostreams/Compression.h"

#include "00_tcp/TcpService.h"
#include "00_tcp/SocketException.h"
#include "00_tcp/Constants.h"
#include "00_tcp/TcpServerSocket.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>

#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>


using namespace boost::posix_time;

using namespace synthese::tcp;
using namespace synthese::util;



namespace synthese
{
    namespace dbring
    {

	NodeServerThreadExec::NodeServerThreadExec (
	    NodeInfo clientNodeInfo,
	    int port,
	    TcpServerSocket* serverSocket, 
	    const NodeInfoMap& nodeInfoMap,
	    TransmissionStatusMap& serverStatusMap)
	    : _clientNodeInfo (clientNodeInfo)
	    , _port (port)
	    , _serverSocket (serverSocket)
	    , _nodeInfoMap (nodeInfoMap)
	    , _serverStatusMap (serverStatusMap)
	{
	}
	


	void 
	NodeServerThreadExec::finalize ()
	{
	}


	void 
	NodeServerThreadExec::loop ()
	{
	    NodeId clientNodeId = _clientNodeInfo.getNodeId ();
	    _serverStatusMap.setTransmissionStatus (clientNodeId, INPROGRESS);

	    bool failed (false);
	    try 
	    {
		_serverSocket->setTimeOut (1000); // 1s timeout

		char buf[512];
		boost::iostreams::stream<tcp::TcpServerSocket> tcpStream (*_serverSocket);

		if (!tcpStream.good ()) throw 1;

		// First, read client last acknowledged timestamp
		tcpStream.getline (buf, sizeof(buf), ETB);
		const boost::posix_time::ptime timestamp (time_from_string (std::string (buf)));

		if (!tcpStream.good ()) throw 1;

		// Now read all pending client update record is 
		std::stringstream pss;
		Compression::ZlibDecompress (tcpStream, pss);
		std::set<uid> clientPendingUpdateRecords;

		if (!tcpStream.good ()) throw 1;


		while (pss.getline (buf, sizeof(buf), ' '))
		{
		    clientPendingUpdateRecords.insert (Conversion::ToLongLong (buf));
		}

		// Now send a complete node info map except recipient
		int nbNodeInfos (0);
		std::stringstream niss;
		for (NodeInfoMap::const_iterator it = _nodeInfoMap.begin ();
		     it != _nodeInfoMap.end (); ++it)
		{
		     // Client is authoritative on its own node info.
		    if (it->first == clientNodeId) continue;
		    niss << it->second << ETB;
		    ++nbNodeInfos;
		}
		tcpStream << nbNodeInfos << ETB << niss.str () << std::flush;
		if (!tcpStream.good ()) throw 1;

		// Now prepare the corresponding update log
		UpdateRecordSet urs;
		UpdateRecordTableSync::LoadDeltaUpdate (urs, timestamp, clientPendingUpdateRecords);

		tcpStream << urs << std::flush;
		if (!tcpStream.good ()) {
		    throw 1;
		}

		if (urs.size () > 0)
		{
		    Log::GetInstance ().info ("Sent " + Conversion::ToString (urs.size ()) + " records since "
					      + to_iso_string (timestamp));
		}

		char c (0);

		tcpStream.get (c);
		if (!tcpStream.good ()) throw 1;

		if (c != ACK)
		{
		    failed = true;
		}
	    }
	    catch (tcp::SocketException& soe) 
	    {
		// silently catched...
		failed = true;
	    }
	    catch (std::exception& e) 
	    {
		Log::GetInstance ().error ("Error during node server loop", e);
		failed = true;
	    }
	    catch (...) 
	    {
		// silently catched...
		failed = true;
	    }

	    _serverStatusMap.setTransmissionStatus (clientNodeId, READY);
		
	    // Close the connection delegated by Node.
	    TcpService::openService (_port)->closeConnection (_serverSocket);


	}
	
    }    
}

