#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H


#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "IncludingPlace.h"

#include <string>
#include <iostream>
#include <vector>


namespace synthese
{
namespace env
{



/** City class.

A city holds in its included places the main connection places
 (those taken by default when no stop is explicitly chosen).

@ingroup m15
*/
class City : public synthese::util::Registrable<uid, City>, 
	         public IncludingPlace
{
 private:

    // TODO add interpretor

 public:

    City (const uid& key,
	  const std::string& name);
    
    ~City ();


    //! @name Getters/Setters
    //@{
    
    //@}

    //! @name Update methods
    //@{

    //@}


 private:


};





}
}
#endif

