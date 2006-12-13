#ifndef SYNTHESE_UTIL_THREADEXCEPTION_H
#define SYNTHESE_UTIL_THREADEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace util
{


/** Thread related exception class
@ingroup m01
*/

class ThreadException : public synthese::util::Exception
{
 private:

 public:

    ThreadException ( const std::string& message);
    ~ThreadException () throw ();

 private:


};




}

}
#endif

