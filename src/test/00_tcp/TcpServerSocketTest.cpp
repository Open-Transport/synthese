#include "TcpServerSocketTest.h"

#include "00_tcp/TcpService.h"
#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"


#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/iostreams/stream.hpp>


namespace synthese
{
namespace tcp
{


  void 
  TcpServerSocketTest::setUp () 
  {

  }


  void 
  TcpServerSocketTest::tearDown () 
  {

  } 


    
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
		CPPUNIT_ASSERT (clientSock.isConnected ());
		
		// The client is connected.

		// Create commodity stream:
		boost::iostreams::stream<TcpClientSocket> cliSocketStream;
		cliSocketStream.open (clientSock);

		// Wait for a message...
		std::string message;
		cliSocketStream >> message;

		std::string expectedMessage ("MessageToClient");
		CPPUNIT_ASSERT_EQUAL (expectedMessage, message);

		// Send a reply
		cliSocketStream << _replyToSend << std::endl;
		
		cliSocketStream.close ();
	    }
    };
    


    

  void 
  TcpServerSocketTest::testSimpleConnection () 
  {
      
      TcpClientThread clientThread ("MessageToServerFromClient0");
      boost::thread client (clientThread);

      TcpService* service = TcpService::openService (8899);

      try 
      {
	  // Wait for client connection...
	  TcpServerSocket& socket = service->acceptConnection ();

	  CPPUNIT_ASSERT_EQUAL (1, service->getConnectionCount ());
	  
	  // Create commodity stream:
	  boost::iostreams::stream<TcpServerSocket> srvSocketStream (socket);

	  // Send a message to client:
	  srvSocketStream << "MessageToClient" << std::endl;

	  // Wait for a reply:
	  std::string reply;
	  srvSocketStream >> reply;

	  std::string expectedReply ("MessageToServerFromClient0");
	  CPPUNIT_ASSERT_EQUAL (expectedReply, reply);

	  
	  srvSocketStream.close ();

	  service->closeConnection (socket);
	  CPPUNIT_ASSERT_EQUAL (0, service->getConnectionCount ());
      } 
      catch (SocketException& se)
      {
	  CPPUNIT_ASSERT (false);
      }
      
      client.join ();


      TcpService::closeService (8899);
  } 



  void 
  TcpServerSocketTest::testSimpleConnectionWhitespaceTransfer () 
  {
      
      TcpClientThread clientThread ("Message to server from client 0");
      boost::thread client (clientThread);

      TcpService* service = TcpService::openService (8899);

      try 
      {
	  // Wait for client connection...
	  TcpServerSocket& socket = service->acceptConnection ();

	  CPPUNIT_ASSERT_EQUAL (1, service->getConnectionCount ());
	  
	  // Create commodity stream:
	  boost::iostreams::stream<TcpServerSocket> srvSocketStream (socket);

	  // Send a message to client:
	  srvSocketStream << "MessageToClient" << std::endl;

	  // Wait for a reply:
	  char buffer[1024*64]; // 64K buffer max
	  srvSocketStream.getline (buffer, 1024*64);
	  std::string reply (buffer);

	  std::string expectedReply ("Message to server from client 0");
	  CPPUNIT_ASSERT_EQUAL (expectedReply, reply);
	  
	  srvSocketStream.close ();

	  service->closeConnection (socket);
	  CPPUNIT_ASSERT_EQUAL (0, service->getConnectionCount ());
      } 
      catch (SocketException& se)
      {
	  CPPUNIT_ASSERT (false);
      }
      
      client.join ();


      TcpService::closeService (8899);
  } 





  void 
  TcpServerSocketTest::testMultipleConnections () 
  {
      TcpService* service = TcpService::openService (8899);

      TcpClientThread clientThread0 ("MessageToServerFromClient0");
      TcpClientThread clientThread1 ("MessageToServerFromClient1");
      TcpClientThread clientThread2 ("MessageToServerFromClient2");

      boost::thread client0 (clientThread0);
      TcpServerSocket& socket0 = service->acceptConnection ();
      CPPUNIT_ASSERT_EQUAL (1, service->getConnectionCount ());

      boost::thread client1 (clientThread1);
      TcpServerSocket& socket1 = service->acceptConnection ();
      CPPUNIT_ASSERT_EQUAL (2, service->getConnectionCount ());

      boost::thread client2 (clientThread2);
      TcpServerSocket& socket2 = service->acceptConnection ();
      CPPUNIT_ASSERT_EQUAL (3, service->getConnectionCount ());

      {
	  boost::iostreams::stream<TcpServerSocket> stream (socket1);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient1");
	  CPPUNIT_ASSERT_EQUAL (expectedReply, reply);
	  client1.join ();
      }
      {
	  boost::iostreams::stream<TcpServerSocket> stream (socket2);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient2");
	  CPPUNIT_ASSERT_EQUAL (expectedReply, reply);
	  client2.join ();
      }
      {
	  boost::iostreams::stream<TcpServerSocket> stream (socket0);
	  stream << "MessageToClient" << std::endl;
	  
	  std::string reply;
	  stream >> reply;
	  std::string expectedReply ("MessageToServerFromClient0");
	  CPPUNIT_ASSERT_EQUAL (expectedReply, reply);
	  client0.join ();
      }

      service->closeConnection (socket0);
      service->closeConnection (socket1);
      service->closeConnection (socket2);

      CPPUNIT_ASSERT_EQUAL (0, service->getConnectionCount ());

      TcpService::closeService (8899);
  } 







}
}

