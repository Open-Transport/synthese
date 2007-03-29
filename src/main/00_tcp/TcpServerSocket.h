#ifndef SYNTHESE_TCP_TCPSERVERSOCKET_H
#define SYNTHESE_TCP_TCPSERVERSOCKET_H


#include "module.h"

#include <boost/iostreams/concepts.hpp> 

#include "SocketException.h"


namespace synthese
{
namespace tcp
{

    class TcpService;

/** TCP server side socket device.

This class implements the concept of Boost Iostreams
bidirectional device. It allows to use stream operators
on the opened TCP socket.

@ingroup m00
*/

class TcpServerSocket 
{
public:

    typedef char char_type;
    typedef boost::iostreams::bidirectional_device_tag category;

private:

    static const int DEFAULT_TIME_OUT = 10;

    TcpService& _service;
    const int _socketId;
    int _timeOut; //!< Time out in seconds.

    
    TcpServerSocket (TcpService& service, 
		     int socketId, 
		     int timeOut = DEFAULT_TIME_OUT);

public:

    ~TcpServerSocket ();

    int getSocketId () const;

    std::streamsize read(char* s, std::streamsize n);

    std::streamsize write(const char* s, std::streamsize n);

    /** If timeOut == 0, operation is blocking.
     */
    void setTimeOut (int timeOut);

    friend class TcpService;

};




}
}
#endif

