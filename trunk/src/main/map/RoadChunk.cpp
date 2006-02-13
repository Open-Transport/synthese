#include "RoadChunk.h"

#include <cmath>
#include "Address.h"
#include "Location.h"
#include "PhysicalStop.h"
#include "Vertex.h"


namespace synmap
{


RoadChunk::RoadChunk(Topography* topography,
		     int key,
		     const std::vector<const Location*>& steps,
		     Address::AddressNumber rightStartNumber, 
		     Address::AddressNumber rightEndNumber, 
		     Address::AddressNumber leftStartNumber, 
		     Address::AddressNumber leftEndNumber)
  : Referrant (topography, key)
    , _steps (steps)
    , _passagePoints (filterPhysicalStops (steps))
    , _rightStartNumber (rightStartNumber)
    , _rightEndNumber (rightEndNumber)
    , _leftStartNumber (leftStartNumber)
    , _leftEndNumber (leftEndNumber)
    , _length (computeTotalLength (_passagePoints))
{
}



RoadChunk::~RoadChunk()
{
}



Address::AddressNumberSide 
RoadChunk::getSideForNumber (Address::AddressNumber number) const
{
  int n = (int) std::floor (number);
  int rsn = (int) std::floor (_rightStartNumber);
  int ren = (int) std::floor (_rightEndNumber);
  int lsn = (int) std::floor (_leftStartNumber);
  int len = (int) std::floor (_leftEndNumber);


  if (rsn % 2 == n % 2) return Address::RIGHT_SIDE;
  if (ren % 2 == n % 2) return Address::RIGHT_SIDE;
  if (lsn % 2 == n % 2) return Address::LEFT_SIDE;
  if (len % 2 == n % 2) return Address::LEFT_SIDE;
  
  return Address::UNKNOWN_SIDE;	
}



bool 
RoadChunk::hasNumber (Address::AddressNumber number) const
{
  Address::AddressNumberSide ans = getSideForNumber(number);

  if (getRightStartNumber() == number) return true;
  if (getRightEndNumber() == number) return true;
  if (getLeftStartNumber() == number) return true;
  if (getLeftEndNumber() == number) return true;
  
  if (ans == Address::RIGHT_SIDE) {

    if (getRightStartNumber() == Address::UNKNOWN_ADDRESS_NUMBER) return false;
    if (getRightEndNumber() == Address::UNKNOWN_ADDRESS_NUMBER) return false;

    if ((getRightStartNumber() <= number) &&
	(getRightEndNumber() >= number)) return true; 
    
  } else if (ans == Address::LEFT_SIDE) {
    
    if (getLeftStartNumber() == Address::UNKNOWN_ADDRESS_NUMBER) return false;
    if (getLeftEndNumber() == Address::UNKNOWN_ADDRESS_NUMBER) return false;

    if ((getLeftStartNumber() <= number) &&
	(getLeftEndNumber() >= number)) return true; 
    
  }
  return false;
}



double 
RoadChunk::computeTotalLength (const std::vector<const Vertex*>& passagePoints) {
  double totalLength = 0.0;
  for (int i=0; i+1<passagePoints.size (); ++i) {
    totalLength += 
      passagePoints[i]->distanceTo (*(passagePoints[i+1]));
  }
  return totalLength;
}



std::vector<const Vertex*> 
RoadChunk::filterPhysicalStops (const std::vector<const Location*>& steps) 
{
  std::vector<const Vertex*> passagePoints;
  for (std::vector<const Location*>::const_iterator iter (steps.begin());
       iter != steps.end ();
       ++iter) {
    const Location* location = *iter;
    const PhysicalStop* physicalStop = dynamic_cast<const PhysicalStop*> (location);
    if (!physicalStop) {
      passagePoints.push_back (location->getVertex ());
    } 
  }
  return passagePoints;
  

}


void 
RoadChunk::setRoad (const Road* road) { 
  _road = road; 


  // ceci est la suite de la bidouille infame commencee dans
  // physical stop: on profite de l'occasion pour leur affecter leur
  // point metrique et leur route, ce qui va creer son adresse. infect.
	
  double metricOffset = 0.0;
  const Vertex* previousVertex = 0;
  for (std::vector<const Location*>::const_iterator iter (_steps.begin());
       iter != _steps.end ();
       ++iter) {
    const Location* location = *iter;
    
    const PhysicalStop* physicalStop = dynamic_cast<const PhysicalStop*> (location);
    if (!physicalStop) {
      if (previousVertex != 0) {
	metricOffset += previousVertex->distanceTo (*location->getVertex ());
	previousVertex = location->getVertex ();
    } else {
      // TODO : on neglige la distance entre les deux vertex de passage pour l'instant
      // faire l'export des coordonnnes projetees depuis intranet...
      ((PhysicalStop*) physicalStop)->setMetricOffset (_road, metricOffset);
    }
  }

}
}





}
