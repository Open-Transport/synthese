#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "City.h"


#include <string>
#include <iostream>



namespace synthese
{
namespace env
{



/** 
@ingroup m15
*/
class Environment
{
 private:

    City::Registry _cities;

    // 



 public:

    Environment ();
    ~Environment ();


    //! @name Getters/Setters
    //@{
    City::Registry& getCities ();
    //@}



 private:


};





}
}
#endif

