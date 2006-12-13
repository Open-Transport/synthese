#include "VertexAccessMap.h"

#include "Vertex.h"
#include "Edge.h"
#include "Line.h"
#include "ConnectionPlace.h"

#include <assert.h>
#include <set>
#include <limits>



namespace synthese
{
namespace env
{





VertexAccessMap::VertexAccessMap ()
    : _isobarycenterUpToDate (false)
    , _isobarycenterMaxSquareDistanceUpToDate (false)
    , _minApproachTime (std::numeric_limits<int>::max ())
{
    
}




VertexAccessMap::~VertexAccessMap ()
{

}




const VertexAccess& 
VertexAccessMap::getVertexAccess (const Vertex* vertex) const
{
    assert (contains (vertex));
    return _map.find (vertex)->second;
}






bool 
VertexAccessMap::needFineSteppingForDeparture (const Path* path) const
{
    std::map<const Path*, bool>::const_iterator it = 
	_fineSteppingForDeparture.find (path);
    
    if (it == _fineSteppingForDeparture.end()) return false;
    return it->second;
}




bool 
VertexAccessMap::needFineSteppingForArrival (const Path* path) const
{
    std::map<const Path*, bool>::const_iterator it = 
	_fineSteppingForArrival.find (path);
    
    if (it == _fineSteppingForArrival.end()) return false;
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
    updateFineSteppingVertexMap ();
    _isobarycenterUpToDate = false;
    _isobarycenterMaxSquareDistanceUpToDate = false;
    if (vertexAccess.approachTime < _minApproachTime)
    {
	_minApproachTime = vertexAccess.approachTime;
    }

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
VertexAccessMap::updateFineSteppingVertexMap ()
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
		_fineSteppingForDeparture[line] = 
		    (it->first->getConnectionPlace () == 0) ||
		    (it->first->getConnectionPlace ()->getConnectionType () == 
		     ConnectionPlace::CONNECTION_TYPE_ROADROAD);
	    }
	}

	for (std::set<const Edge*>::const_iterator itEdge = it->first->getArrivalEdges ().begin ();
	     itEdge != it->first->getArrivalEdges ().end (); ++itEdge)
	{
	    const Line* line = dynamic_cast<const Line*> ((*itEdge)->getParentPath ());
	    if (line != 0)
	    {
		_fineSteppingForDeparture[line] = 
		    (it->first->getConnectionPlace () == 0) ||
		    (it->first->getConnectionPlace ()->getConnectionType () == 
		     ConnectionPlace::CONNECTION_TYPE_ROADROAD);
	    }
	}
	
    }
}




int 
VertexAccessMap::getMinApproachTime () const
{
    return _minApproachTime;
}




const std::map<const Vertex*, VertexAccess>& 
VertexAccessMap::getMap () const
{
    return _map;
}



void 
VertexAccessMap::merge (const VertexAccessMap& vam,
			MergeAddresses mergeAddresses,
			MergePhysicalStops mergePhysicalStops)
{
    for (std::map<const Vertex*, VertexAccess>::const_iterator itps = vam.getMap ().begin ();
	 itps != vam.getMap ().end (); ++itps)
    {
	if ( (mergeAddresses == MERGE_ADDRESSES) && 
	     (itps->first->isAddress () == true) )
	{
	    insert (itps->first, itps->second);
	}
	if ( (mergePhysicalStops == MERGE_PHYSICALSTOPS) && 
	     (itps->first->isAddress () == false) )
	{
	    insert (itps->first, itps->second);
	}
    }

}




}
}

