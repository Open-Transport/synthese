#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "City.h"
#include "Document.h"
#include "Line.h"
#include "LogicalStop.h"


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
    LogicalStop::Registry _logicalStops;



 public:

    Environment ();
    ~Environment ();


    //! @name Getters/Setters
    //@{
    City::Registry& getCities ();
    const City::Registry& getCities () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    Line::Registry& getLines ();
    const Line::Registry& getLines () const;

    LogicalStop::Registry& getLogicalStops ();
    const LogicalStop::Registry& getLogicalStops () const;
    // ...
    //@}



 private:


};





}
}
#endif

