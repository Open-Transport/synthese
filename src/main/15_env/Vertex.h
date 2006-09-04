#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H

#include <set>


#include "Point.h"

#include "module.h"


namespace synthese
{
namespace env
{


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
    
    // TODO : when needed add a _containingPlace to be able, for instance,
    // to display the public place associated with OR change _connectionPlace into
    // _addressablePlace and let the getConnectionPlace accesor do the dynamic_cast.

    
    const ConnectionPlace* _connectionPlace;
    int _rankInConnectionPlace;

    std::set<const Edge*> _departureEdges; //!< Departure edges from this physical stop
    std::set<const Edge*> _arrivalEdges; //!< Arrival edges to this physical stop

protected:

    Vertex (const ConnectionPlace* connectionPlace,
	    int rankInConnectionPlace,
	    double x = UNKNOWN_VALUE,
	    double y = UNKNOWN_VALUE);

public:

    virtual ~Vertex ();


    //! @name Getters/Setters
    //@{
    const ConnectionPlace* getConnectionPlace () const;  
    int getRankInConnectionPlace () const;

    const std::set<const Edge*>& getDepartureEdges () const;
    const std::set<const Edge*>& getArrivalEdges () const;

    //@}


    //! @name Update methods
    //@{
    void addDepartureEdge ( const Edge* edge );
    void addArrivalEdge ( const Edge* edge );
     //@}


    //! @name Query methods
    //@{

    //@}
    
    
};


}
}

#endif 	    
