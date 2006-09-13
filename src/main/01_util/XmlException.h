#ifndef SYNTHESE_UTIL_XMLEXCEPTION_H
#define SYNTHESE_UTIL_XMLEXCEPTION_H


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace util
{


/** Xml related exception class
@ingroup m01
*/

class XmlException : public synthese::util::Exception
{
 private:

 public:

    XmlException ( const std::string& message);
    ~XmlException () throw ();

 private:


};




}

}
#endif
