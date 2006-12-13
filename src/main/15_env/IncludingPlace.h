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

    void getImmediateVertices (VertexAccessMap& result, 
			       const AccessDirection& accessDirection,
			       const AccessParameters& accessParameters,
			       const Vertex* origin = 0,
			       bool returnAddresses = true,
			       bool returnPhysicalStops = true) const;

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

