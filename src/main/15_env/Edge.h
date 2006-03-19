#ifndef SYNTHESE_ENV_EDGE_H
#define SYNTHESE_ENV_EDGE_H



namespace synthese
{
namespace env
{

    class Path;
    class Vertex;


/** Edge abstract base class.

An edge is an oriented arc between two vertices.
An edge necessarily belongs to a path.

For instance :
- if the edge is a road chunk, its parent path is a road.
- if the edge is a line stop, its parent path is a line.

 @ingroup m15
*/
class Edge
{

private:


protected:

    Edge ();

public:

    virtual ~Edge ();


    //! @name Getters/Setters
    //@{
    virtual const Path* getParentPath () const = 0;
    virtual const Vertex* getFrom () const = 0;
    virtual const Vertex* getTo () const = 0;
    //@}

    
};


}
}

#endif 	    
