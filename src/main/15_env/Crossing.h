#ifndef SYNTHESE_ENV_CROSSING_H
#define SYNTHESE_ENV_CROSSING_H


#include "01_util/Registrable.h"
#include "ConnectionPlace.h"
#include <string>


namespace synthese
{
namespace env
{




/** Crossing class.

A crossing defines a connection place between addresses from
different roads.

 @ingroup m15
*/
class Crossing : public synthese::util::Registrable<int, Crossing>, public ConnectionPlace
{

private:

protected:

    Crossing (const int& id,
	      const std::string& name,
	      const City* city,
	      const ConnectionType& connectionType = CONNECTION_TYPE_AUTHORIZED);


public:

    virtual ~Crossing ();


    //! @name Getters/Setters
    //@{

    //@}

    
};


}
}

#endif 	    
