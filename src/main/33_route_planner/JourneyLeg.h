#ifndef SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H
#define SYNTHESE_ROUTEPLANNER_JOURNEYLEG_H

#include "04_time/DateTime.h"


#include <string>


namespace synthese
{

namespace env
{
    class Service;
    class Vertex;
}

namespace routeplanner
{



/** Journey leg class.


@ingroup m33
*/
class JourneyLeg
{
 private:

    const synthese::env::Vertex* const _origin;   //!< Origin
    const synthese::env::Vertex* const _destination;  //!< Destination

    const synthese::time::DateTime _departureTime; //!< Departure moment (first if continuous service)
    const synthese::time::DateTime _arrivalTime;    //!< Arrival moment (first if continuous service)
    const synthese::env::Service* _service; //!< Used service.
    

 public:


    JourneyLeg (const synthese::env::Vertex* origin,
		const synthese::env::Vertex* destination,
		const synthese::time::DateTime& departureTime,
		const synthese::time::DateTime& arrivalTime,
		const synthese::env::Service* service);

    ~JourneyLeg ();


    //! @name Getters/Setters
    //@{
    const synthese::env::Service* getService () const;

    const synthese::time::DateTime& getDepartureTime () const;
    const synthese::time::DateTime& getArrivalTime () const;

    const synthese::env::Vertex* getOrigin () const;
    const synthese::env::Vertex* getDestination () const;
    
    //@}



    //! @name Update methods
    //@{

    //@}


 private:


};





}
}
#endif

