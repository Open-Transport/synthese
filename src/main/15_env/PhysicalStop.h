#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H


#include "Vertex.h"
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
    
 public:

    PhysicalStop (const std::string& name, 
		  int rankInLogicalStop,
		  const LogicalStop* logicalStop);

    ~PhysicalStop ();
    

    //! @name Getters/Setters
    //@{
    //@}



};



}
}



#endif
