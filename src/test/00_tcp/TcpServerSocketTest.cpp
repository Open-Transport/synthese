#include "01_util/Conversion.h"
#include "01_util/iostreams/Compression.h"
#include "01_util/threads/Thread.h"

#include "00_tcp/TcpService.h"
#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"


#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::tcp;
using namespace synthese::util;
using namespace boost::iostreams;

struct TcpClientThread
{
    std::string _replyToSend;

    TcpClientThread (const std::string& replyToSend) 
	: _replyToSend (replyToSend)
	{
	}
	
    void operator()()
	{
	    TcpClientSocket clientSock ("localhost", 8899);
		
	    while (clientSock.isConnected () == false)
	    {
		clientSock.tryToConnect ();
		boost::thread::yield ();
	    }
	    BOOST_CHECK (clientSock.isConnected ());
		
	    // The client is connected.

	    // Create commodity stream:
	    boost::iostreams::stream<TcpClientSocket> cliSocketStream;
	    cliSocketStream.open (clientSock);

	    // Wait for a message...
	    std::string message;
	    cliSocketStream >> message;

	    std::string expectedMessage ("MessageToClient");
	    BOOST_CHECK_EQUAL (expectedMessage, message);

	    // Send a reply
	    cliSocketStream << _replyToSend << std::endl;
		
	    cliSocketStream.close ();
	}
};
    


BOOST_AUTO_TEST_CASE (testSimpleConnection)
{
    TcpClientThread clientThread ("MessageToServerFromClient0");
    boost::thread client (clientThread);
    
    TcpService* service = TcpService::openService (8899, true, false);
    
    try 
    {
	// Wait for client connection...
	TcpServerSocket* socket = service->acceptConnection ();

	BOOST_CHECK_EQUAL (1, service->getConnectionCount ());
	
	// Create commodity stream:
	boost::iostreams::stream<TcpServerSocket> srvSocketStream (*socket);
	
	// Send a message to client:
	srvSocketStream << "MessageToClient" << std::endl;
	
	// Wait for a reply:
	std::string reply;
	srvSocketStream >> reply;
	
	std::string expectedReply ("MessageToServerFromClient0");
	BOOST_CHECK_EQUAL (expectedReply, reply);
	
	
	srvSocketStream.close ();
	
	service->closeConnection (socket);
	BOOST_CHECK_EQUAL (0, service->getConnectionCount ());
    } 
    catch (SocketException& se)
    {
	BOOST_CHECK (false);
    }
    
    client.join ();
    
    
    TcpService::closeService (8899);
} 



BOOST_AUTO_TEST_CASE (testSimpleConnectionWhitespaceTransfer)
{

      TcpClientThread clientThread ("Message to server from client 0");
      boost::thread client (clientThread);

      TcpService* service = TcpService::openService (8899, true, false);

      try 
      {
	  // Wait for client connection...
	  TcpServerSocket* socket = service->acceptConnection ();

	  BOOST_CHECK_EQUAL (1, service->getConnectionCount ());
	  
	  // Create commodity stream:
	  boost::iostreams::stream<TcpServerSocket> srvSocketStream (*socket);

	  // Send a message to client:
	  srvSocketStream << "MessageToClient" << std::endl;

	  // Wait for a reply:
	  char buffer[1024*64]; // 64K buffer max
	  srvSocketStream.getline (buffer, 1024*64);
	  std::string reply (buffer);

	  std::string expectedReply ("Message to server from client 0");
	  BOOST_CHECK_EQUAL (expectedReply, reply);
	  
	  srvSocketStream.close ();

	  service->closeConnection (socket);
	  BOOST_CHECK_EQUAL (0, service->getConnectionCount ());
      } 
      catch (SocketException& se)
      {
	  BOOST_CHECK (false);
      }
      
      client.join ();


      TcpService::closeService (8899);
  } 





BOOST_AUTO_TEST_CASE (testMultipleConnections)
{
    TcpService* service = TcpService::openService (8899, true, false);

      TcpClientThread clientThread0 ("MessageToServerFromClient0");
      TcpClientThread clientThread1 ("MessageToServerFromClient1");
      TcpClientThread clientThread2 ("MessageToServerFromClient2");

      boost::thread client0 (clientThread0);
      TcpServerSocket* socket0 = service->acceptConnection ();
      BOOST_CHECK_EQUAL (1, service->getConnectionCount ());

      boost::thread client1 (clientThread1);
      TcpServerSocket* socket1 = service->acceptConnection ();
      BOOST_CHECK_EQUAL (2, service->getConnectionCount ());

      boost::thread client2 (clientThread2);
      TcpServerSocket* socket2 = service->acceptConnection ();
      BOOST_CHECK_EQUAL (3, service->getConnectionCount ());

      {
	  boost::iostreams::stream<TcpServerSocket> stream (*socket1);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient1");
	  BOOST_CHECK_EQUAL (expectedReply, reply);
	  client1.join ();
      }
      {
	  boost::iostreams::stream<TcpServerSocket> stream (*socket2);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient2");
	  BOOST_CHECK_EQUAL (expectedReply, reply);
	  client2.join ();
      }
      {
	  boost::iostreams::stream<TcpServerSocket> stream (*socket0);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient0");
	  BOOST_CHECK_EQUAL (expectedReply, reply);
	  client0.join ();
      }

      service->closeConnection (socket0);
      service->closeConnection (socket1);
      service->closeConnection (socket2);

      BOOST_CHECK_EQUAL (0, service->getConnectionCount ());

      TcpService::closeService (8899);
  } 





struct TcpZlibClientThread
{
    std::string _replyToSend;

    TcpZlibClientThread (const std::string& replyToSend) 
	: _replyToSend (replyToSend)
	{
	}
	
    void operator()()
	{

	    try
	    {
		TcpClientSocket clientSock ("localhost", 8899, 0);
		
		while (clientSock.isConnected () == false)
		{
		    clientSock.tryToConnect ();
		    boost::thread::yield ();
		}
		
		BOOST_CHECK (clientSock.isConnected ());
		
		// The client is connected.

		stream<TcpClientSocket> ss (clientSock);
	    
		std::stringstream msg;
		Compression::ZlibDecompress (ss, msg);

		std::string expectedMessage ("MessageToClient");
		BOOST_CHECK_EQUAL (expectedMessage, msg.str ());
		
                // Send a reply
		msg.str (_replyToSend);
		Compression::ZlibCompress (msg, ss);


	    }
	    catch (std::exception& e)
	    {
		BOOST_FAIL (e.what ());
	    }
	}
};
    


BOOST_AUTO_TEST_CASE (testSimpleConnectionWithCompression)
{
    TcpZlibClientThread clientThread ("MessageToServerFromClient0");
    boost::thread client (clientThread);
    
    TcpService* service = TcpService::openService (8899, true, true);
    
    try 
    {
	// Wait for client connection...
	TcpServerSocket* socket (0);
	while (!socket) socket = service->acceptConnection ();

	BOOST_CHECK_EQUAL (1, service->getConnectionCount ());

	stream<TcpServerSocket> ss (*socket);

	// Send a compressed message to client:
	std::stringstream msg ("MessageToClient");
	Compression::ZlibCompress (msg, ss);

	// Wait for a reply
	msg.str("");
	Compression::ZlibDecompress (ss, msg);

	BOOST_CHECK_EQUAL ("MessageToServerFromClient0", msg.str ());
	service->closeConnection (socket);
	BOOST_CHECK_EQUAL (0, service->getConnectionCount ());
    } 
    catch (SocketException& se)
    {
	BOOST_FAIL (se.what ());
    }
    
    client.join ();
    
    
    TcpService::closeService (8899);
} 
