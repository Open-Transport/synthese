#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "Vertex.h"
#include "module.h"
#include "BroadcastPoint.h"

#include <string>


namespace synthese
{
namespace env
{

 class LineStop;
 class ConnectionPlace;
 

/** Physical stop (bus stop, etc.).
    A physical stop is an entry point to the transport network.
 @ingroup m15
*/
class PhysicalStop : 
    public synthese::util::Registrable<uid,PhysicalStop>,
    public Vertex,
	public BroadcastPoint
{

private:

    std::string _name; //! Physical stop name
    
 public:

    PhysicalStop (const uid& id,
		  const std::string& name, 
		  const AddressablePlace* place,
		  double x = UNKNOWN_VALUE,
		  double y = UNKNOWN_VALUE);

    ~PhysicalStop ();
    

    //! @name Getters/Setters
    //@{
    const uid& getId () const;

    const std::string& getName () const;
    void setName (const std::string& name);
    //@}


    //! @name Query methods
    //@{
    bool isAddress () const;
    bool isPhysicalStop () const;
    //@}

};



}
}



#endif
