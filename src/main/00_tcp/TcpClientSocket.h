
/** TcpClientSocket class header.
	@file TcpClientSocket.h

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

#ifndef SYNTHESE_TCP_TCPCLIENTSOCKET_H
#define SYNTHESE_TCP_TCPCLIENTSOCKET_H



#include <boost/iostreams/concepts.hpp> 
#include <boost/shared_ptr.hpp>

#include "Socket.h"
#include "SocketException.h"


namespace synthese
{
namespace tcp
{

  
/** TCP client side socket device.

This class implements the concept of Boost Iostreams
bidirectional device. It allows to use stream operators
on the opened TCP socket.

@ingroup m00
*/

class TcpClientSocket 
{
public:

    typedef char char_type;
    typedef boost::iostreams::bidirectional_device_tag category;

/*    struct category : 
	boost::iostreams::device_tag, 
	boost::iostreams::bidirectional,
	boost::iostreams::multichar_tag
	{ };
*/

private:

    static const int DEFAULT_TIME_OUT = 10000;  // 10s

    const std::string _hostName;
    const int _portNumber;
    const std::string _protocol;
    int _timeOut; //!< Time out in seconds.

    boost::shared_ptr<Socket> _socket;

public:

    /** TCP client socket constructor.
	The connection is not established immediately.
	The connect method has to be called before.

	@param hostName Host to connect
	@param portNumber Remore port to connect.
	@param timeOut Time out
     */
    TcpClientSocket (const std::string& hostName,
		     int portNumber, 
//		     bool tcpProtocol = true,
		     int timeOut = DEFAULT_TIME_OUT);

    ~TcpClientSocket ();

    bool isConnected () const;
    void tryToConnect ();

    /** If timeOut == 0, operation is blocking.
     */
    void setTimeOut (int timeOut);

    std::streamsize read(char* s, std::streamsize n);

    std::streamsize write(const char* s, std::streamsize n);


};




}

}
#endif

