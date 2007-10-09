
/** TcpServerSocket class implementation.
	@file TcpServerSocket.cpp

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

#include "Socket.h"

#include "TcpServerSocket.h"
#include "TcpService.h"



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





void 
TcpServerSocket::setTimeOut (int timeOut)
{
    _timeOut = timeOut;
}



std::streamsize 
TcpServerSocket::read(char* s, std::streamsize n)
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
TcpServerSocket::write(const char* s, std::streamsize n)
{
    try 
    {
	return _service._socket->write (_socketId, s, n, _timeOut);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}









}
}

