#ifndef SYNTHESE_ENV_LOGICALSTOP_H
#define SYNTHESE_ENV_LOGICALSTOP_H


#include <vector>

#include "Registrable.h"
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
    public Registrable<int,LogicalStop>, 
    public ConnectionPlace
{

private:

    std::vector<const PhysicalStop*> _physicalStops; 

protected:

public:


    LogicalStop (const int& id,
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
