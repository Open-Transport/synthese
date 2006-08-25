#ifndef SYNTHESE_ENV_PLACE_H
#define SYNTHESE_ENV_PLACE_H


#include "module.h"


#include <map>
#include <string>
#include <vector>


namespace synthese
{
namespace env
{


 class City;
 class Vertex;


/** Place base class.

Place is the base for any class which is associated with a name
and a city. However, a place is not related to the concept
of geographical location in terms of coordinates; it is rather
a human abstraction.

 @ingroup m15
*/
class Place 
{
public:

    typedef std::vector<const Vertex*> AccessPath;
    typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;


    typedef struct {
	double maxApproachDistance;
	double maxApproachTime;
	double approachSpeed;
	
    } AccessParameters;


    typedef struct {
	AccessPath path;
	double approachTime;
	double approachDistance;
    } VertexAccess;
    
    
    typedef std::map<const Vertex*, VertexAccess> VertexAccessMap;


private:

    std::string _name;  //! The name of this city
    const City* _city; //!< The city where this place is located

protected:

    Place (const std::string& name,
	   const City* city);

public:


    virtual ~Place ();


    //! @name Getters/Setters
    //@{

    /** Gets name of this place.
     */
    const std::string& getName () const;
    void setName (const std::string& name);


    /** Gets official name of this place.
     * The default implementation is to return this name.
     */
    virtual const std::string& getOfficialName () const;


    /** Gets city where this place is located.
     */
    const City* getCity () const;

    //@}



    //! @name Query methods
    //@{

    virtual VertexAccess getVertexAccess (const AccessDirection& accessDirection,
					  const AccessParameters& accessParameters,
					  const Vertex* destination,
					  const Vertex* origin) const;


    virtual void getImmediateVertices (VertexAccessMap& result, 
				       const AccessDirection& accessDirection,
				       const AccessParameters& accessParameters,
				       const Vertex* origin = 0,
				       bool returnAddresses = true,
				       bool returnPhysicalStops = true) const = 0;
    //@}

    
};


}
}

#endif 	    
