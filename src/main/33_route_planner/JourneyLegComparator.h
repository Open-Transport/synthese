#ifndef SYNTHESE_ROUTEPLANNER_JOURNEYLEGCOMPARATOR_H
#define SYNTHESE_ROUTEPLANNER_JOURNEYLEGCOMPARATOR_H


#include "15_env/module.h"


namespace synthese
{

namespace env
{
    class Edge;
}

namespace routeplanner
{

    class JourneyLeg;


/** Journey leg comparator class.


@ingroup m33
*/
class JourneyLegComparator
{
 private:

    typedef const synthese::env::Edge* (JourneyLeg::*PtrEdgeAccessor) () const;
    PtrEdgeAccessor _edgeAccessor;

 public:


    JourneyLegComparator (const synthese::env::AccessDirection& accessDirection);
    ~JourneyLegComparator ();

    int operator () (const JourneyLeg* jl1, const JourneyLeg* jl2) const;
    
};



}
}
#endif


