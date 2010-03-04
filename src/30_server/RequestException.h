#ifndef SYNTHESE_SERVER_REQUESTEXCEPTION_H
#define SYNTHESE_SERVER_REQUESTEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace server
{


/** Request related exception class
@ingroup m15
*/

class RequestException : public synthese::util::Exception
{
 private:

 public:

    RequestException ( const std::string& message) throw();
    ~RequestException () throw ();

 private:


};




}

}
#endif

