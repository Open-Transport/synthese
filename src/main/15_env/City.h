#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H


#include "Registrable.h"


#include <string>
#include <iostream>



namespace synthese
{
namespace env
{



/** 
@ingroup m15
*/
class City : public Registrable<int, City>
{
 private:
    

 public:

    City (const int& key);
    ~City ();



 private:


};





}
}
#endif

