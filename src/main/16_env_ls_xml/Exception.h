#ifndef SYNTHESE_ENVLSXML_EXCEPTION_H
#define SYNTHESE_ENVLSXML_EXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace envlsxml
{


/** Load/save related exception class
@ingroup m70
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

