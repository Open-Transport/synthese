#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "City.h"
#include "Document.h"
#include "Line.h"


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
    Document::Registry _documents;
    Line::Registry _lines;
    


 public:

    Environment ();
    ~Environment ();


    //! @name Getters/Setters
    //@{
    City::Registry& getCities ();
    Document::Registry& getDocuments ();
    Line::Registry& getLines ();
    // ...
    //@}



 private:


};





}
}
#endif

