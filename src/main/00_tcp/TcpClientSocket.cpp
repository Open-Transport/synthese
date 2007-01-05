#include "TcpClientSocket.h"


namespace synthese
{
namespace tcp
{


TcpClientSocket::TcpClientSocket (const std::string& hostName,
				  int portNumber, 
//				  bool tcpProtocol,
				  int timeOut) 
    : _hostName (hostName)
    , _portNumber (portNumber)
    , _protocol (/*tcpProtocol*/ true ? PROTOCOL_TYPE_TCP 
		             : PROTOCOL_TYPE_UDP)
    , _timeOut (timeOut)
    , _socket ()
{
    // @todo : apprently the low level socket class doe NOT work properly with UDP!
    // Fixed to TCP right now

}



    
TcpClientSocket::~TcpClientSocket ()
{
}





bool 
TcpClientSocket::isConnected () const
{
    return _socket.get () != 0;
}






void 
TcpClientSocket::tryToConnect () throw (SocketException)
{
	Socket* socket = new Socket ();
    try 
    {
	socket->open (_hostName.c_str (),
		      _portNumber, 
		      _protocol.c_str () );
    }
    catch (const char* msg)
    {
	delete socket;
	throw SocketException (msg);
    }

    try 
    {
	socket->connectToServer();
    } 
    catch (const char* msg)
    {
	// This is not a cause of exception raising.
	delete socket;
	return;
    }
    _socket.reset (socket);
}







void 
TcpClientSocket::setTimeOut (int timeOut)
{
    _timeOut = timeOut;
}




std::streamsize 
TcpClientSocket::read(char* s, std::streamsize n) throw (SocketException)
{
    try 
    {
	return _socket->read (s, n, _timeOut);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}






std::streamsize 
TcpClientSocket::write(const char* s, std::streamsize n) throw (SocketException)
{
    try 
    {
	return _socket->write (s, n, 0);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}






}
}



