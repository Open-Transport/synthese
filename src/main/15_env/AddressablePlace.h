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

private:

    std::vector<const Address*> _addresses; 


protected:

    AddressablePlace (const std::string& name,
		      const City* city);


public:

    virtual ~AddressablePlace ();

    //! @name Getters/Setters
    //@{

    /** Gets addresses of this place.
     */
    const std::vector<const Address*>& getAddresses () const;
    
    //@}


    //! @name Query methods
    //@{

    /** Launches a full navigation of the road network from the 
     *  set of associated addresses, 
     *
     * @param accessDirection Is this place an origin or a destination.
     * @param result The shortest accesses to included places physical stops, or 
     *               one physical stop accesses of an arbitrarily chosen 
     *		     connection place of this city.
     */
    void reachPhysicalStopAccesses (const AccessDirection& accessDirection,
				    const AccessParameters& accessParameters,
				    PhysicalStopAccessMap& result) const;

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
