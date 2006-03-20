#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H

#include <set>
#include "Point.h"


namespace synthese
{
namespace env
{


    class ConnectionPlace;
    class Edge;


/** Vertex base class.

A vertex is the abstract graph node used for route planning.

 @ingroup m15
*/
class Vertex : public Point
{

private:

    const ConnectionPlace* _connectionPlace;
    int _rankInConnectionPlace;

    std::set<const Edge*> _departureEdges; //!< Departure edges from this physical stop
    std::set<const Edge*> _arrivalEdges; //!< Arrival edges to this physical stop

protected:

    Vertex (const ConnectionPlace* connectionPlace,
	    int rankInConnectionPlace);

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


    
};


}
}

#endif 	    
