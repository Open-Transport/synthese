#include "Edge.h"




namespace synthese
{
namespace env
{



Edge::Edge (const EdgeType& type) 
    : _type (type)
{
    
}


Edge::~Edge ()
{
    // Delete via points
    for (std::vector<const Point*>::iterator iter = _viaPoints.begin (); 
	 iter != _viaPoints.end (); 
	 ++iter)
    {
	delete (*iter);
    }
    _viaPoints.clear ();
    
}



const Edge::EdgeType&
Edge::getType () const
{
    return _type;
}



void 
Edge::setType ( const EdgeType& type )
{
    _type = type;
}



bool 
Edge::isArrival () const
{
    return ( _type == EDGE_TYPE_PASSAGE || _type == EDGE_TYPE_ARRIVAL );
}



bool 
Edge::isDeparture () const
{
    return ( _type == EDGE_TYPE_PASSAGE || _type == EDGE_TYPE_DEPARTURE );
}



const Edge* 
Edge::getNextInPath () const
{
    return _nextInPath;
}



void 
Edge::setNextInPath (const Edge* nextInPath)
{
    _nextInPath = nextInPath;
}




const Edge* 
Edge::getPreviousDeparture () const
{
    return _previousDeparture;
}



const Edge* 
Edge::getFollowingArrival () const
{
    return _followingArrival;
}


const Edge* 
Edge::getPreviousConnectionDeparture () const
{
    return _previousConnectionDeparture;
}



const Edge* 
Edge::getFollowingConnectionArrival () const
{
    return _followingConnectionArrival;
}




void 
Edge::setPreviousDeparture ( const Edge* previousDeparture)
{
    _previousDeparture = previousDeparture;
}




void 
Edge::setPreviousConnectionDeparture( const Edge* previousConnectionDeparture)
{
    _previousConnectionDeparture = previousConnectionDeparture;
}




void 
Edge::setFollowingArrival ( const Edge* followingArrival)
{
    _followingArrival = followingArrival;
}




void 
Edge::setFollowingConnectionArrival( const Edge* followingConnectionArrival)
{
    _followingConnectionArrival = followingConnectionArrival;
}




const std::vector<const Point*>& 
Edge::getViaPoints () const
{
    return _viaPoints;
}



void 
Edge::addViaPoint (const Point& viaPoint)
{
    _viaPoints.push_back (new Point (viaPoint));
}



}
}
