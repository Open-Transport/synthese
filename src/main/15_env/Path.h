#ifndef SYNTHESE_ENV_PATH_H
#define SYNTHESE_ENV_PATH_H


#include "BikeComplyer.h"
#include "HandicappedComplyer.h"
#include "PedestrianComplyer.h"
#include "ReservationRuleComplyer.h"
#include "Calendar.h"

#include <vector>


namespace synthese
{

namespace time
{
    class DateTime;
}

namespace env
{

    class Alarm;
    class Edge;
    class Fare;
    class Point;
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
class Path : 
    public BikeComplyer,
    public HandicappedComplyer,
    public PedestrianComplyer,
    public ReservationRuleComplyer
    
{

private:

protected:

    std::vector<Edge*> _edges; 
    std::vector<Service*> _services;

    Fare* _fare;
    Alarm* _alarm;

    // TODO remonter egalement les chaninages    

    Calendar _calendar; //!< Calendar indicating if there is at least one service running on each day.

    Path ();

public:

    virtual ~Path ();


    //! @name Getters/Setters
    //@{
    int getEdgesCount () const;
    const Edge* getEdge (int index) const;

    const std::vector<Service*>& getServices () const;
    const Service* getService (int serviceNumber) const;

    const Fare* getFare () const;
    void setFare (Fare* fare);

    const Alarm* getAlarm() const;

    const std::vector<Edge*>& getEdges () const;

    //@}

    //! @name Query methods.
    //@{

    /** Gets all the geographical points linked by the path
        between two of its edges. If no from/to edge
	index is provided, all the edges are considered.
	@param fromEdgeIndex 
	@param toEdgeIndex 

	This includes :
	- vertices (address/physical stops)
	- via points
    */
    std::vector<const Point*> getPoints (int fromEdgeIndex = 0,
					 int toEdgeIndex = -1) const;

    bool isInService (const synthese::time::Date& date) const;

    //@}
    
    //! @name Update methods.
    //@{
    void addEdge (Edge* edge);
    void addService (Service* service);

    void postInit ();


    /** Updates path calendar.

    The generated calendar indicates whether or not a day contains at least one service.
    It takes into account services running after midnight : if at least one minute
    of a day is concerned by a service, then the whole day is selected.

    Thus, if a calculation request is done on a deselected calendar day, the path 
    can safely be filtered.
    */
    void updateCalendar ();
    //@}


    //@}
    
    
};


}
}

#endif 	    
