#ifndef SYNTHESE_ENV_EXCEPTION_H
#define SYNTHESE_ENV_EXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace env
{


/** Environment related exception class
@ingroup m15
*/

class Exception : public synthese::util::Exception
{
 private:

 public:

    Exception ( const std::string& message);
    ~Exception () throw ();

 private:


};




}

}
#endif
