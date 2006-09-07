#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H

#include <set>


#include "01_util/UId.h"
#include "Point.h"

#include "module.h"


namespace synthese
{
namespace env
{


    class AddressablePlace;
    class ConnectionPlace;
    class Edge;
    class Path;


/** Vertex base class.

A vertex is the abstract graph node used for route planning.
A vertex is necessarily connected to its corresponding path.
 @ingroup m15
*/
class Vertex : public Point
{

private:
    
    const AddressablePlace* _addressablePlace;

    std::set<const Edge*> _departureEdges; //!< Departure edges from this physical stop
    std::set<const Edge*> _arrivalEdges; //!< Arrival edges to this physical stop

protected:

    Vertex (const AddressablePlace* place,
	    double x = UNKNOWN_VALUE,
	    double y = UNKNOWN_VALUE);

public:

    virtual ~Vertex ();


    //! @name Getters/Setters
    //@{
    const AddressablePlace* getPlace () const;  

    const ConnectionPlace* getConnectionPlace () const;  

    const std::set<const Edge*>& getDepartureEdges () const;
    const std::set<const Edge*>& getArrivalEdges () const;

    virtual const uid& getId () const = 0;

    //@}


    //! @name Update methods
    //@{
    void addDepartureEdge ( const Edge* edge );
    void addArrivalEdge ( const Edge* edge );
     //@}


    //! @name Query methods
    //@{
    virtual bool isAddress () const = 0;
    virtual bool isPhysicalStop () const = 0;
    //@}
    
    
};


}
}

#endif 	    
