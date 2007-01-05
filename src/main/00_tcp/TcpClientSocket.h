#ifndef SYNTHESE_TCP_TCPCLIENTSOCKET_H
#define SYNTHESE_TCP_TCPCLIENTSOCKET_H


#include "module.h"

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

private:

    static const int DEFAULT_TIME_OUT = 10;

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
    void tryToConnect () throw (SocketException);

    /** If timeOut == 0, operation is blocking.
     */
    void setTimeOut (int timeOut);

    std::streamsize read(char* s, std::streamsize n) throw (SocketException);

    std::streamsize write(const char* s, std::streamsize n) throw (SocketException);


};




}

}
#endif

