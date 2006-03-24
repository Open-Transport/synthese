#ifndef SYNTHESE_ENV_ENVIRONMENT_H
#define SYNTHESE_ENV_ENVIRONMENT_H


#include "Axis.h"
#include "City.h"
#include "Document.h"
#include "Line.h"
#include "LogicalStop.h"
#include "PhysicalStop.h"


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

    Axis::Registry _axes;
    City::Registry _cities;
    Document::Registry _documents;
    Line::Registry _lines;
    LogicalStop::Registry _logicalStops;
    PhysicalStop::Registry _physicalStops;



 public:

    Environment ();
    ~Environment ();


    //! @name Getters/Setters
    //@{
    Axis::Registry& getAxes ();
    const Axis::Registry& getAxes () const;

    City::Registry& getCities ();
    const City::Registry& getCities () const;

    Document::Registry& getDocuments ();
    const Document::Registry& getDocuments () const;

    Line::Registry& getLines ();
    const Line::Registry& getLines () const;

    LogicalStop::Registry& getLogicalStops ();
    const LogicalStop::Registry& getLogicalStops () const;

    PhysicalStop::Registry& getPhysicalStops ();
    const PhysicalStop::Registry& getPhysicalStops () const;
    // ...
    //@}



 private:


};





}
}
#endif

