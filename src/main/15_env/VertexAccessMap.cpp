#include "VertexAccessMap.h"

#include "Vertex.h"
#include "Edge.h"
#include "Line.h"
#include "ConnectionPlace.h"

#include <set>




namespace synthese
{
namespace env
{





VertexAccessMap::VertexAccessMap ()
    : _isobarycenterUpToDate (false)
    , _isobarycenterMaxSquareDistanceUpToDate (false)
{
    
}




VertexAccessMap::~VertexAccessMap ()
{

}




const std::map<const Vertex*, VertexAccess>& 
VertexAccessMap::getMap () const
{
    return _map;
}




bool 
VertexAccessMap::hasNonLineConnectableDepartureVertex (const Line* line) const
{
    std::map<const Line*, bool>::const_iterator it = 
	_nonLineConnectableDepartureVertex.find (line);
    
    if (it == _nonLineConnectableDepartureVertex.end()) return false;
    return it->second;
}




bool 
VertexAccessMap::hasNonLineConnectableArrivalVertex (const Line* line) const
{
    std::map<const Line*, bool>::const_iterator it = 
	_nonLineConnectableArrivalVertex.find (line);
    
    if (it == _nonLineConnectableArrivalVertex.end()) return false;
    return it->second;
}




bool 
VertexAccessMap::contains (const Vertex* vertex) const
{
    return (_map.find (vertex) != _map.end ());
}




void 
VertexAccessMap::insert (const Vertex* vertex, 
			 const VertexAccess& vertexAccess)
{
    _map.insert (std::make_pair (vertex, vertexAccess));
    updateNonLineConnectableVertexMap ();
    _isobarycenterUpToDate = false;
    _isobarycenterMaxSquareDistanceUpToDate = false;

}





const Point& 
VertexAccessMap::getIsobarycenter () const
{
    if (_isobarycenterUpToDate == false)
    {
	double sumx (0.0);
	double sumy (0.0);
	
	for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
	     it != _map.end (); ++it)
	{
	    sumx += it->first->getX ();
	    sumy += it->first->getY ();
	}
	
	_isobarycenter.setX (sumx / ((double) _map.size ()));
	_isobarycenter.setY (sumy / ((double) _map.size ()));
	
	_isobarycenterUpToDate = true;
    }
    return _isobarycenter;
    
}


const SquareDistance&
VertexAccessMap::getIsobarycenterMaxSquareDistance () const
{
    if (_isobarycenterMaxSquareDistanceUpToDate == false)
    {
	_isobarycenterMaxSquareDistance.setSquareDistance (0);
	for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
	     it != _map.end (); ++it)
	{
	    SquareDistance sqd (*(it->first), _isobarycenter);
	    if (_isobarycenterMaxSquareDistance < sqd)
	    {
		_isobarycenterMaxSquareDistance.setSquareDistance (sqd.getSquareDistance ());
	    }
	}
	_isobarycenterMaxSquareDistanceUpToDate = true;
    }
    return _isobarycenterMaxSquareDistance;
    
}






void 
VertexAccessMap::updateNonLineConnectableVertexMap ()
{
    // TODO : make it lazy... on demand

    for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
	 it != _map.end (); ++it)
    {
	for (std::set<const Edge*>::const_iterator itEdge = it->first->getDepartureEdges ().begin ();
	     itEdge != it->first->getDepartureEdges ().end (); ++itEdge)
	{
	    const Line* line = dynamic_cast<const Line*> ((*itEdge)->getParentPath ());
	    if (line != 0)
	    {
		_nonLineConnectableDepartureVertex[line] = 
		    (it->first->getConnectionPlace () == 0) ||
		    (it->first->getConnectionPlace ()->getConnectionType () == 
		     ConnectionPlace::CONNECTION_TYPE_ROADONLY);
	    }
	}

	for (std::set<const Edge*>::const_iterator itEdge = it->first->getArrivalEdges ().begin ();
	     itEdge != it->first->getArrivalEdges ().end (); ++itEdge)
	{
	    const Line* line = dynamic_cast<const Line*> ((*itEdge)->getParentPath ());
	    if (line != 0)
	    {
		_nonLineConnectableDepartureVertex[line] = 
		    (it->first->getConnectionPlace () == 0) ||
		    (it->first->getConnectionPlace ()->getConnectionType () == 
		     ConnectionPlace::CONNECTION_TYPE_ROADONLY);
	    }
	}
	
    }
}







}
}
