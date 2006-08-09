#ifndef SYNTHESE_ENV_PLACE_H
#define SYNTHESE_ENV_PLACE_H


#include <map>
#include <string>


namespace synthese
{
namespace env
{


 class City;
 class PhysicalStop;


/** Place base class.

Place is the base for any class which is associated with a name
and a city. However, a place is not related to the concept
of geographical location in terms of coordinates; it is rather
a human abstraction.

 @ingroup m15
*/
class Place
{

private:

    std::string _name;  //! The name of this city
    const City* _city; //!< The city where this place is located

protected:

    Place (const std::string& name,
	   const City* city);

public:

    typedef struct {
	double approachTime;
    } PhysicalStopAccess;


    typedef struct {
	double maxDistance;
    } AccessParameters;


    typedef std::map<const PhysicalStop*, PhysicalStopAccess> PhysicalStopAccessMap;

    typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;

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


    /** Collects all physical stop accesses according to the place
     * type and the access direction.
     */
    virtual void reachPhysicalStopAccesses (const AccessDirection& accessDirection,
					    const AccessParameters& accessParameters,
					    PhysicalStopAccessMap& result) const = 0;

    /** Gets city where this place is located.
     */
    const City* getCity () const;

    //@}

    
};


}
}

#endif 	    
