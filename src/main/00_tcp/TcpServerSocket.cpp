#include "TcpServerSocket.h"
#include "TcpService.h"
#include "Socket.h"



namespace synthese
{
namespace tcp
{


TcpServerSocket::TcpServerSocket (TcpService& service, 
				  int socketId, 
				  int timeOut)
    : _service (service)
    , _socketId (socketId)
    , _timeOut (timeOut)
{
}




TcpServerSocket::~TcpServerSocket ()
{
}




int 
TcpServerSocket::getSocketId () const
{
    return _socketId;
}




std::streamsize 
TcpServerSocket::read(char* s, std::streamsize n) throw (SocketException)
{
    try 
    {
	return _service._socket->read (_socketId, s, n, _timeOut);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}




std::streamsize 
TcpServerSocket::write(const char* s, std::streamsize n) throw (SocketException)
{
    try 
    {
	return _service._socket->write (_socketId, s, n, 0);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}









}
}
