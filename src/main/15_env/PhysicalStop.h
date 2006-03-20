#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H


#include "Vertex.h"
#include <set>
#include <string>


namespace synthese
{
namespace env
{

 class LineStop;
 class LogicalStop;


/** Physical stop (bus stop, etc.).
    A physical stop is an entry point to the transport network.
 @ingroup m15
*/
class PhysicalStop : 
    public Vertex
{

private:

    const std::string _name; //! Physical stop name
    std::set<const LineStop*> _departureLineStops; //!< Departure line stops of the physical stop
    std::set<const LineStop*> _arrivalLineStops; //!< Arrival line stops of the physical stop
    
 public:

    PhysicalStop (const std::string& name, 
		  int rankInLogicalStop,
		  const LogicalStop* logicalStop);

    ~PhysicalStop ();
    

    //! @name Getters/Setters
    //@{
    const std::set<const LineStop*>& 
	getDepartureLineStops () const;

    const std::set<const LineStop*>& 
	getArrivalLineStops () const;

    //@}


    //! @name Update methods
    //@{
    void addDepartureLineStop ( const LineStop* lineStop );
    void addArrivalLineStop ( const LineStop* lineStop );
     //@}


};



}
}



#endif
