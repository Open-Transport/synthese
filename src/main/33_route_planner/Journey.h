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
    class Vertex;
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
    const synthese::env::Vertex* getOrigin() const;
    const synthese::env::Vertex* getDestination() const;

    const synthese::time::DateTime& getDepartureTime () const;
    const synthese::time::DateTime& getArrivalTime () const;

    //@}



    //! @name Update methods
    //@{
    void clear ();
    void prepend (const JourneyLeg* leg);
    void append (const JourneyLeg* leg);
    
    //@}



};





}
}
#endif

