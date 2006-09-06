#ifndef SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H
#define SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H

#include "04_time/DateTime.h"
#include "15_env/SquareDistance.h"


#include <string>


namespace synthese
{

namespace env
{
    class Axis;
    class Service;
    class Edge;
    class Path;
}

namespace routeplanner
{



/** Journey leg class.


@ingroup m33
*/
class JourneyLeg
{
 private:

    const synthese::env::Edge* _origin;   //!< Origin
    const synthese::env::Edge* _destination;  //!< Destination

    synthese::time::DateTime _departureTime; //!< Departure moment (first if continuous service)
    synthese::time::DateTime _arrivalTime;    //!< Arrival moment (first if continuous service)
    const synthese::env::Service* _service; //!< Used service.

    int _continuousServiceRange;
    
    synthese::env::SquareDistance _squareDistance;

 public:


    JourneyLeg ();
    ~JourneyLeg ();


    //! @name Getters/Setters
    //@{
    const synthese::env::Path* getPath () const;
    
    const synthese::env::Service* getService () const;
    void setService (const synthese::env::Service* service);

    const synthese::time::DateTime& getDepartureTime () const;
    void setDepartureTime (const synthese::time::DateTime& departureTime);

    const synthese::time::DateTime& getArrivalTime () const;
    void setArrivalTime (const synthese::time::DateTime& arrivalTime);

    const synthese::env::Edge* getOrigin () const;
    void setOrigin (const synthese::env::Edge* origin);

    const synthese::env::Edge* getDestination () const;
    void setDestination (const synthese::env::Edge* destination);

    int getContinuousServiceRange () const;
    void setContinuousServiceRange (int continuousServiceRange);

    const synthese::env::SquareDistance& getSquareDistance () const;
    synthese::env::SquareDistance& getSquareDistance ();
    void setSquareDistance (const synthese::env::SquareDistance& squareDistance);


    //@}


    //! @name Query methods
    //@{
    const synthese::env::Axis* getAxis () const;

    /** Returns this journey leg duration in minutes.
     */
    int getDuration () const;

    int getDistance () const;
    //@}


    //! @name Update methods
    //@{

    //@}


 private:


};





}
}
#endif

