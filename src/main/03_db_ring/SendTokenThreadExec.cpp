#include "03_db_ring/SendTokenThreadExec.h"
#include "03_db_ring/Constants.h"

#include "01_util/Log.h"
#include "01_util/iostreams/Compression.h"
#include "00_tcp/TcpClientSocket.h"

#include <boost/iostreams/stream.hpp>


using namespace synthese::tcp;
using namespace synthese::util;



namespace synthese
{
    namespace dbring
    {

	    SendTokenThreadExec::SendTokenThreadExec (
		const NodeId& emitterNodeId,
		const NodeInfo& nodeInfo, 
		const TokenSPtr token, 
		TransmissionStatusMap& transmissionStatusMap) 

		: _emitterNodeId (emitterNodeId)
		, _nodeInfo (nodeInfo)
		, _transmissionStatusMap (transmissionStatusMap)
	    {
		// Serialize token to send
		_tokenBuffer << (*token);
	    }

	    
	    void 
	    SendTokenThreadExec::loop ()
	    {
		_transmissionStatusMap.setTransmissionStatus (_nodeInfo.getNodeId (), INPROGRESS);

		// Do send token
		TcpClientSocket clientSock (_nodeInfo.getHost (), _nodeInfo.getPort (), 5*10*TCP_TOKEN_TIMEOUT );
		clientSock.tryToConnect ();

		if (clientSock.isConnected ()) 
		{
		    try
		    {
			boost::iostreams::stream<TcpClientSocket> cliSocketStream (clientSock);
			Compression::ZlibCompress (_tokenBuffer, cliSocketStream);
			cliSocketStream.close ();
			_transmissionStatusMap.setTransmissionStatus (_nodeInfo.getNodeId (), SUCCESS);
			
 			  std::stringstream logstr;
 			  logstr << "Node " << _emitterNodeId << " sent [" << _tokenBuffer.str ().size () << " bytes" <<  
			      /* << _tokenBuffer.str () << */ "] to node " << _nodeInfo.getNodeId () << std::endl;
			  
 			  Log::GetInstance ().debug (logstr.str ());
			
			return;
		    }
		    catch (std::exception& e) 
		    {
			Log::GetInstance ().error ("Error sending token", e);
		    }
		    catch (...) 
		    {
			
			Log::GetInstance ().error ("Unknown error sending token");
		    }
		}
		_transmissionStatusMap.setTransmissionStatus (_nodeInfo.getNodeId (), FAILURE);
	    }


	}
    
}

