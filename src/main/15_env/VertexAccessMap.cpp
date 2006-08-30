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




void 
VertexAccessMap::insert (const Vertex* vertex, 
			 const VertexAccess& vertexAccess)
{
    _map.insert (std::make_pair (vertex, vertexAccess));
    updateNonLineConnectableVertexMap ();
    updateIsobarycenter ();
    updateIsobarycenterMaxDistance ();    
}





void 
VertexAccessMap::updateNonLineConnectableVertexMap ()
{
    // TODO : confirm that connection_roadonly means that connection contains at most
    // one line and then only raods.

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





void 
VertexAccessMap::updateIsobarycenter ()
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
    

}



void 
VertexAccessMap::updateIsobarycenterMaxDistance ()
{
    _isobarycenterMaxDistance = 0.0;
    double distance (0.0);
    for (std::map<const Vertex*, VertexAccess>::const_iterator it = _map.begin ();
	 it != _map.end (); ++it)
    {
	distance = it->first->distanceTo (_isobarycenter);
	if (distance > _isobarycenterMaxDistance)
	{
	    _isobarycenterMaxDistance = distance;
	}
    }
}





}
}
