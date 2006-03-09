#ifndef SYNTHESE_UTIL_CONVERSION_H
#define SYNTHESE_UTIL_CONVERSION_H


#include "module.h"

#include <string>
#include <iostream>



namespace synthese
{
namespace util
{

/** Service class for basic conversions.
@ingroup m01
*/
class Conversion
{
 private:

    Conversion ();
    ~Conversion ();

    static char _Buffer[128];

 public:

    static int ToInt (std::string s);

    static std::string ToString (int i);


};



}

}
#endif
