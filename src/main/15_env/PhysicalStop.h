#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "Vertex.h"
#include "module.h"

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
    public synthese::util::Registrable<uid,PhysicalStop>,
    public Vertex
{

private:

    const std::string _name; //! Physical stop name
    
 public:

    PhysicalStop (const uid& id,
		  const std::string& name, 
		  int rankInLogicalStop,
		  const LogicalStop* logicalStop,
		  double x = UNKNOWN_VALUE,
		  double y = UNKNOWN_VALUE);

    ~PhysicalStop ();
    

    //! @name Getters/Setters
    //@{
    const std::string& getName () const;
    //@}



};



}
}



#endif
