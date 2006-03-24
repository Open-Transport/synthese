#ifndef SYNTHESE_ENV_EDGE_H
#define SYNTHESE_ENV_EDGE_H



#include <vector>
#include "Point.h"


namespace synthese
{
namespace env
{

    class Path;
    class Vertex;


/** Edge abstract base class.

An edge necessarily belongs to a path.

For instance :
- if the edge is a road chunk, its parent path is a road.
- if the edge is a line stop, its parent path is a line.

An edge holds links between one vertex (the "from" vertex),
and several others edges sharing the same path.

This way, a path can be traversed following different views :
- departure/arrival view
- departure/arrival connection view

An edge is said to be :
- a departure edge if it is possible to start a move from it;
- an arrival edge if it is possible to end a move to it;
- a passage edge if it can be considered both as a departure edge 
  and as an arrival edge.


 @ingroup m15
*/
class Edge
{
public:

    typedef enum 
	{
            EDGE_TYPE_DEPARTURE = 'D',
            EDGE_TYPE_ARRIVAL = 'A',
            EDGE_TYPE_PASSAGE = 'P'
	} EdgeType;


private:


    EdgeType _type;      //!< Departure, arrival or passage    

    const Edge* _previousDeparture;  //!< Previous departure edge along path.
    const Edge* _previousConnectionDeparture; //!< Previous connection departure edge along path.
    const Edge* _followingArrival;  //!< Next arrival edge along path.
    const Edge* _followingConnectionArrival; //!< Next connection arrival edge along path.

    std::vector<Point> _viaPoints; //!< Intemediate points along the edge.

protected:

    Edge (const EdgeType& type);

public:

    virtual ~Edge ();


    //! @name Getters/Setters
    //@{

    virtual const Path* getParentPath () const = 0;

    /** Returns this edge origin vertex.
     */
    virtual const Vertex* getFromVertex () const = 0;

    /** Returns metric offset of this edge from
	parent path origin vertex.
    */
    virtual double getMetricOffset () const = 0;

    const EdgeType& getType () const;
    void setType ( const EdgeType& type );


    const Edge* getPreviousDeparture () const;
    void setPreviousDeparture ( const Edge* previousDeparture);

    const Edge* getPreviousConnectionDeparture () const;
    void setPreviousConnectionDeparture( const Edge* previousConnectionDeparture);

    const Edge* getFollowingArrival () const;
    void setFollowingArrival ( const Edge* followingArrival);

    const Edge* getFollowingConnectionArrival () const;
    void setFollowingConnectionArrival( const Edge* followingConnectionArrival);

    //@}


    //! @name Query methods
    //@{
    bool isArrival () const;
    bool isDeparture () const;
    
    //@}

    //! @name Update methods
    //@{
    void addViaPoint (const Point& viaPoint);
    
    //@}



    
};


}
}

#endif 	    
