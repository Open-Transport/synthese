#ifndef SYNTHESE_TCP_SOCKETEXCEPTION_H
#define SYNTHESE_TCP_SOCKETEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace tcp
{


/** Socket related exception class
@ingroup m70
*/

class SocketException : public synthese::util::Exception
{
 private:

 public:

    SocketException ( const std::string& message);
    ~SocketException () throw ();

 private:


};




}

}
#endif

