#ifndef SYNTHESE_ENV_ADDRESSABLEPLACE_H
#define SYNTHESE_ENV_ADDRESSABLEPLACE_H


#include <vector>

#include "Place.h"


namespace synthese
{
namespace env
{


 class Address;


/** Addressable place base class.

AddressablePlace is the base for any place which can provide
addresses. 

 @ingroup m15
*/
class AddressablePlace : public Place
{
public:

    typedef enum { 
	CONNECTION_TYPE_FORBIDDEN = 0,         //!< neither road connection nor line connection
	CONNECTION_TYPE_ROADROAD = 1,          //!< only road to road connection
	CONNECTION_TYPE_ROADLINE = 2,          //!< only road to line, or line to road, or road to road
	CONNECTION_TYPE_LINELINE = 3,          //!< any connection possible
	CONNECTION_TYPE_RECOMMENDED_SHORT = 4, //!< any connection possible, recommended if short journey
	CONNECTION_TYPE_RECOMMENDED = 5        //!< any connection possible, recommended for any journey
    } ConnectionType;


private:



protected:

    std::vector<const Address*> _addresses; 


    AddressablePlace (const std::string& name,
		      const City* city);


public:

    virtual ~AddressablePlace ();

    //! @name Getters/Setters
    //@{

    /** Gets addresses of this place.
     */
    const std::vector<const Address*>& getAddresses () const;

    virtual const ConnectionType getConnectionType () const;

    //@}


    //! @name Query methods
    //@{

    virtual void getImmediateVertices (VertexAccessMap& result, 
				       const AccessDirection& accessDirection,
				       const AccessParameters& accessParameters,
				       const Vertex* origin = 0,
				       bool returnAddresses = true,
				       bool returnPhysicalStops = true) const;

    //@}




    //! @name Update methods.
    //@{

    /** Adds an address to this place.
     */
    void addAddress (const Address* address);
    //@}


};


}
}

#endif 	    
