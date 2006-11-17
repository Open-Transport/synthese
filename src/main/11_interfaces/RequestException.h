#ifndef SYNTHESE_SERVER_REQUESTEXCEPTION_H
#define SYNTHESE_SERVER_REQUESTEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace interfaces
{


/** Request related exception class
@ingroup m70
*/

class RequestException : public synthese::util::Exception
{
 private:

 public:

    RequestException ( const std::string& message);
    ~RequestException () throw ();

 private:


};




}

}
#endif
