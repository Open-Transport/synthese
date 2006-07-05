#ifndef SYNTHESE_ENV_LOGICALSTOP_H
#define SYNTHESE_ENV_LOGICALSTOP_H


#include <vector>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "ConnectionPlace.h"


namespace synthese
{
namespace env
{


class PhysicalStop;


/** Logical stop class.

 @ingroup m15
*/
class LogicalStop : 
    public synthese::util::Registrable<uid,LogicalStop>, 
    public ConnectionPlace
{

private:

    std::vector<const PhysicalStop*> _physicalStops; 

protected:

public:


    LogicalStop (const uid& id,
		 const std::string& name,
		 const City* city);


    virtual ~LogicalStop ();

    //! @name Getters/Setters
    //@{
    const std::vector<const PhysicalStop*>& getPhysicalStops () const;
    //@}



    //! @name Update methods.
    //@{
    void addPhysicalStop (const PhysicalStop* physicalStop);
    //@}


};


}
}

#endif 	    
