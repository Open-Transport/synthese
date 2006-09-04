#ifndef SYNTHESE_ROUTEPLANNER_JOURNEYLEGCOMPARATOR_H
#define SYNTHESE_ROUTEPLANNER_JOURNEYLEGCOMPARATOR_H


#include "15_env/module.h"


namespace synthese
{

namespace env
{
    class Vertex;
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

    typedef const synthese::env::Vertex* (JourneyLeg::*PtrVertexAccessor) () const;
    PtrVertexAccessor _vertexAccessor;

 public:


    JourneyLegComparator (const synthese::env::AccessDirection& accessDirection);
    ~JourneyLegComparator ();

    int operator () (const JourneyLeg* jl1, const JourneyLeg* jl2) const;
    
};



}
}
#endif

