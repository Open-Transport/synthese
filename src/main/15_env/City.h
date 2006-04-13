#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H


#include "Registrable.h"
#include "IncludingPlace.h"

#include <string>
#include <iostream>
#include <vector>


namespace synthese
{
namespace env
{

    class LogicalStop;


/** City class.

A city holds in its included places the main logical 
stops (those taken by default when no stop is explicitly chosen).

@ingroup m15
*/
class City : public Registrable<int, City>, 
	         public IncludingPlace
{
 private:

    // TODO add interpretor

 public:

    City (const int& key,
	  const std::string& name);

    ~City ();


    //! @name Getters/Setters
    //@{
    
    //@}

    //! @name Update methods
    //@{

    /** Adds a main logical stop to this city.
     */
    void addMainLogicalStop (const LogicalStop* logicalStop);
    //@}


 private:


};





}
}
#endif

