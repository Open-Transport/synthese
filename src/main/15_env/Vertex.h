#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H


#include "Point.h"


namespace synthese
{
namespace env
{


    class ConnectionPlace;


/** Vertex base class.

A vertex is the abstract graph node used for route planning.

 @ingroup m15
*/
class Vertex : public Point
{

private:

    const ConnectionPlace* _connectionPlace;
    int _rankInConnectionPlace;

protected:

    Vertex (const ConnectionPlace* connectionPlace,
	    int rankInConnectionPlace);

public:

    virtual ~Vertex ();


    //! @name Getters/Setters
    //@{
    const ConnectionPlace* getConnectionPlace () const;  
    int getRankInConnectionPlace () const;
    
    //@}

    
};


}
}

#endif 	    
