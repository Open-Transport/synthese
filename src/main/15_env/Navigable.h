#ifndef SYNTHESE_ENV_NAVIGABLE_H
#define SYNTHESE_ENV_NAVIGABLE_H

#include <vector>
#include <map>

namespace synthese
{
namespace env
{

    class PhysicalStop;
    class Vertex;


/** Base class for any part of the network graph that can be 
    navigated.
 @ingroup m15
*/
class Navigable 
{

 protected:
    
    Navigable () {};

 public:

    typedef std::vector<const Vertex*> AccessPath;
    typedef enum { FROM_ORIGIN, TO_DESTINATION } AccessDirection ;

    typedef struct {
	AccessPath path;
	double approachTime;
	double approachDistance;
    } PhysicalStopAccess;
    
    
    typedef struct {
	double maxApproachDistance;
	double maxApproachTime;
	double approachSpeed;
	
    } AccessParameters;
	    

    typedef std::map<const PhysicalStop*, PhysicalStopAccess> PhysicalStopAccessMap;
    
    virtual ~Navigable () {};


    //! @name Query methods
    //@{

    virtual void reachPhysicalStopAccesses (
	const AccessDirection& accessDirection,
	const AccessParameters& accessParameters,
	PhysicalStopAccessMap& result,
	const PhysicalStopAccess& currentAccess = PhysicalStopAccess ()) const = 0;

    //@}


};

}
}


#endif
