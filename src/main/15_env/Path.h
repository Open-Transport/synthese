#ifndef SYNTHESE_ENV_PATH_H
#define SYNTHESE_ENV_PATH_H


#include "Regulated.h"

#include <vector>


namespace synthese
{

namespace time
{
    class DateTime;
}

namespace env
{

    class Edge;
    class Service;

/** Path abstract base class.

A path is a sequence of edges.

A path is associated with a set of services allowing
to follow this path at certain dates and times.

It is associated as well with a set of compliances, defining
which types of entities are able to move along this path. For instance : 
- a bus line is compliant with pedestrians, may be compliant with bikes
  and wheelchairs
- a road may be compliant only with cars, or only for pedestrians and bikes,
  or for all
- a ferry line is compliant with cars, bikes, pedestrian
- ...

 @ingroup m15
*/
class Path : public Regulated
{

private:

protected:

    std::vector<Service*> _services;


    Path ();

public:

    virtual ~Path ();


    //! @name Getters/Setters
    //@{
    virtual int getEdgesCount () const = 0;
    virtual const Edge* getEdge (int index) const = 0;

    const std::vector<Service*>& getServices () const;
    const Service* getService (int serviceNumber) const;
    //@}

    //! @name Query methods.
    //@{
    //@}
    
    //! @name Update methods.
    //@{
    void addService (Service* service);
    //@}
    
    
};


}
}

#endif 	    
