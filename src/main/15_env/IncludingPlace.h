#ifndef SYNTHESE_ENV_INCLUDINGPLACE_H
#define SYNTHESE_ENV_INCLUDINGPLACE_H


#include <vector>

#include "Place.h"


namespace synthese
{
namespace env
{


/** Base class for a place including other places.

 @ingroup m15
*/
class IncludingPlace : public Place
{
private:


protected:

    std::vector<const Place*> _includedPlaces; 

public:

    virtual ~IncludingPlace ();


    //! @name Getters/Setters
    //@{

    /** Gets included places.
     */
    const std::vector<const Place*>& getIncludedPlaces () const;
    
    //@}


    //! @name Query methods
    //@{

    /** Collects all physical stop accesses according to the included
     *  places.
     *  This method iterates over included places, and calls recursively
     *  this method. Child results are compiled into one result map so that
     *  only the shortest access to a given physical stop is kept.
     *
     * @param accessDirection Is this place an origin or a destination.
     * @param result The shortest accesses to included places physical stops.
     */
    virtual void reachPhysicalStopAccesses (const AccessDirection& accessDirection,
					    const AccessParameters& accessParameters,
					    PhysicalStopAccessMap& result) const;
    
    //@}


    //! @name Update methods.
    //@{

    /** Adds an included place to this place.
     */
    void addIncludedPlace (const Place* place);

    //@}


protected:

    IncludingPlace (const std::string& name,
		      const City* city);

 private:




};


}
}

#endif 	    
