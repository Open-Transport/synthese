#ifndef SYNTHESE_ROUTEPLANNER_JOURNEY_H
#define SYNTHESE_ROUTEPLANNER_JOURNEY_H


#include <deque>


namespace synthese
{

namespace time
{
    class DateTime;
}
namespace env
{
    class Edge;
}

namespace routeplanner
{

    class JourneyLeg;

/** Journey class.


@ingroup m33
*/
class Journey
{
 private:

    std::deque<const JourneyLeg*> _journeyLegs;
    int _continuousServiceRange;
    int _effectiveTransportDuration;
    int _transportConnectionCount;
    int _distance;

 public:


    Journey ();
    
    ~Journey ();


    //! @name Getters/Setters
    //@{
    int getJourneyLegCount () const;
    const JourneyLeg* getJourneyLeg (int index) const;

    const JourneyLeg* getFirstJourneyLeg () const;
    const JourneyLeg* getLastJourneyLeg () const;
    //@}

    //! @name Query methods
    //@{
    const synthese::env::Edge* getOrigin() const;
    const synthese::env::Edge* getDestination() const;

    const synthese::time::DateTime& getDepartureTime () const;
    const synthese::time::DateTime& getArrivalTime () const;

    int getDuration () const;
    int getDistance () const;

    int getTransportConnectionCount ();

    /** Returns the effective amount of time spent
	in transport, excluding tranfer delays.
    */
    int getEffectiveTransportDuration () const;

    /** Detects max alarm level in journey.
      
        For each journey leg, 4 cases are possible :
	- Alert on origin
	- Service with reservation rule
	- Service alert
	- Alert on destination
      
	@return Maximum journey alert level.
    */
    int getMaxAlarmLevel () const;


    /** Continuous service range of this journey.
	@return Range duration in minutes, or 0 if unique service.
    */
    int getContinuousServiceRange () const;
    void setContinuousServiceRange (int continuousServiceRange);

    //@}



    //! @name Update methods
    //@{
    void clear ();

    void prepend (const JourneyLeg* leg);
    void prepend (const Journey& journey);

    void append (const JourneyLeg* leg);
    void append (const Journey& journey);
    
    //@}


    int operator > (const Journey& op) const;
    Journey& operator = (const Journey& ref);


};





}
}
#endif

