

/** TcpClientSocket class implementation.
	@file TcpClientSocket.cpp

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


#include "00_tcp/TcpClientSocket.h"

#include "00_tcp/Constants.h"


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
//    std::cerr << "destroying client socket" << std::endl;
}





bool 
TcpClientSocket::isConnected () const
{
    return _socket.get () != 0;
}



   

void 
TcpClientSocket::tryToConnect ()
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
TcpClientSocket::read(char* s, std::streamsize n) 
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
TcpClientSocket::write(const char* s, std::streamsize n)
{
    try 
    {
	return _socket->write (s, n, _timeOut);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}






}
}



