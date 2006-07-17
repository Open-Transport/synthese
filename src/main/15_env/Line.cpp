#include "Line.h"

#include "Service.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "ConnectionPlace.h"


namespace synthese
{
namespace env
{





Line::Line (const uid& id,
	    const std::string& name, 
	    const Axis* axis)
    : synthese::util::Registrable<uid,Line> (id)
    , Path ()
    , _name (name)
    , _axis (axis)
    , _network (0)
    , _rollingStock (0)
    , _isWalkingLine (false)
    , _useInDepartureBoards (true)
    , _useInTimetables (true)
    , _useInRoutePlanning (true)
    , _color (0, 0, 0)
{

}





Line::~Line ()
{
}



const std::string& 
Line::getName () const
{
    return _name;
}





bool 
Line::getUseInDepartureBoards () const
{
    return _useInDepartureBoards;
}



void 
Line::setUseInDepartureBoards (bool useInDepartureBoards)
{
    _useInDepartureBoards = useInDepartureBoards;
}




bool 
Line::getUseInTimetables () const
{
    return _useInTimetables;
}



void 
Line::setUseInTimetables (bool useInTimetables)
{
    _useInTimetables = useInTimetables;
}




bool 
Line::getUseInRoutePlanning () const
{
    return _useInRoutePlanning;
}




void 
Line::setUseInRoutePlanning (bool useInRoutePlanning)
{
    _useInRoutePlanning = useInRoutePlanning;
}





const Axis* 
Line::getAxis () const
{
    return _axis;
}






const std::string& 
Line::getDirection () const
{
    return _direction;
}



void 
Line::setDirection (const std::string& direction)
{
    _direction = direction;
}




const std::string& 
Line::getImage () const
{
    return _image;
}



void 
Line::setImage (const std::string& image)
{
    _image = image;
}




const std::string& 
Line::getShortName () const
{
    return _shortName;
}



void 
Line::setShortName (const std::string& shortName)
{
    _shortName = shortName;
}



	
const std::string& 
Line::getLongName () const
{
    return _longName;
}



void 
Line::setLongName (const std::string& longName)
{
    _longName = longName;
}




const std::string& 
Line::getTimetableName () const
{
    return _timetableName;
}



void 
Line::setTimetableName (const std::string& timetableName)
{
    _timetableName = timetableName;
}





const TransportNetwork* 
Line::getNetwork () const
{
    return _network;
}



void 
Line::setNetwork (TransportNetwork* network)
{
    _network = network;
}



    
const std::string& 
Line::getStyle () const
{
    return _style;
}



void 
Line::setStyle (const std::string& style)
{
    _style = style;
}



	


const RollingStock* 
Line::getRollingStock () const
{
    return _rollingStock;
}



void 
Line::setRollingStock (RollingStock* rollingStock)
{
    _rollingStock = rollingStock;
}




const std::vector<LineStop*>& 
Line::getLineStops() const
{
    return _lineStops;
}



std::vector<const Point*> 
Line::getPoints (int fromLineStopIndex,
		     int toLineStopIndex) const
{
    if (toLineStopIndex == -1) toLineStopIndex = _lineStops.size () - 1;
    std::vector<const Point*> points;
    
    for (int i=fromLineStopIndex; i<=toLineStopIndex; ++i)
    {
	// Adds the line physical stop
	points.push_back (_lineStops[i]->getFromVertex ());
	
	// Adds all the via points of the line stop
	const std::vector<const Point*>& viaPoints = _lineStops[i]->getViaPoints ();
	for (std::vector<const Point*>::const_iterator it = viaPoints.begin (); 
	     it != viaPoints.end (); 
	     ++it)
	{
	    points.push_back (*it);
	}
    }
    return points;
}








void 
Line::postInit ()
{
    for ( std::vector<LineStop*>::const_iterator iter = _lineStops.begin();
	  iter != _lineStops.end();
	  ++iter )
    {
        LineStop* lineStop = *iter;

        if ( lineStop->getFollowingArrival () == 0 )
            lineStop->setType ( Edge::EDGE_TYPE_ARRIVAL );
        if ( lineStop->getPreviousDeparture () == 0 )
            lineStop->setType ( Edge::EDGE_TYPE_DEPARTURE );
    }
}



void 
Line::addLineStop (LineStop* lineStop)
{
    if (_lineStops.empty () == false)
    {
	_lineStops.back ()->setNextInPath (lineStop);
    }

    _lineStops.push_back( lineStop );

    if ( _lineStops.size() > 1 )
    {
        // Chaining departure/arrival
        for ( std::vector<LineStop*>::reverse_iterator riter = _lineStops.rbegin();
	      ( riter != _lineStops.rend() )
		  && (
		      ( *riter )->getFollowingArrival () == 0
		      || ( *riter )->getFollowingConnectionArrival() == 0
		      || lineStop->getPreviousDeparture () == 0
		      || lineStop->getPreviousConnectionDeparture() == 0
		      );
	      ++riter )
        {
            LineStop* currentLineStop = *riter;
         
	    // Chain only relations between A and A, D and D, A and D 
	    // if different stops, D and A if different stops
            if ( currentLineStop->getFromVertex ()->getConnectionPlace() != 
		 lineStop->getFromVertex ()->getConnectionPlace() || 
		 currentLineStop->getType () == lineStop->getType () )
            {
                // Chain following arrivals
                if ( currentLineStop->getFollowingArrival () == 0 && 
		     lineStop->isArrival () )
		{
                    currentLineStop->setFollowingArrival ( lineStop );
		}
		if ( currentLineStop->getFollowingConnectionArrival () == 0 && 
		     lineStop->getFromVertex ()->getConnectionPlace()
		     ->isConnectionAuthorized () )
		{
                    currentLineStop->setFollowingConnectionArrival ( lineStop );
		} 
                if ( currentLineStop->isDeparture () && 
		     lineStop->getPreviousDeparture () == 0 )
		{   
		    lineStop->setPreviousDeparture ( currentLineStop );
		}

		if ( currentLineStop->isDeparture () && 
		     lineStop->getPreviousConnectionDeparture () == 0 && 
		     currentLineStop->getFromVertex ()
		     ->getConnectionPlace()->isConnectionAuthorized() )
		{
                    lineStop->setPreviousConnectionDeparture ( currentLineStop );
		}
            }
        }
    }
    
}








void
Line::setWalkingLine (bool isWalkingLine)
{
    _isWalkingLine = isWalkingLine;
}



bool 
Line::getWalkingLine () const
{
    return _isWalkingLine;
}






int 
Line::getEdgesCount () const
{
    return _lineStops.size ();
}


const Edge* 
Line::getEdge (int index) const
{
    return _lineStops[index];
}




const synthese::util::RGBColor& 
Line::getColor () const
{
    return _color;
}



void 
Line::setColor (const synthese::util::RGBColor& color)
{
    _color = color;
}





}
}
