#include "RoadChunk.h"

namespace synmap
{

RoadChunk::RoadChunk(Topography* topography,
		     int key,
		     const std::vector<const Location*>& steps,
		     AddressNumber rightStartNumber, 
		     AddressNumber rightEndNumber, 
		     AddressNumber leftStartNumber, 
		     AddressNumber leftEndNumber)
  : Referrant (topography, key)
  , _rightStartNumber (rightStartNumber)
  , _rightEndNumber (rightEndNumber)
  , _leftStartNumber (leftStartNumber)
  , _leftEndNumber (leftEndNumber)
{
}



RoadChunk::~RoadChunk()
{
}


RoadChunk::AddressNumberSide 
RoadChunk::getSideForNumber (AddressNumber number) const
{
  if (_rightStartNumber % 2 == number % 2) return RIGHT_SIDE;
  if (_rightEndNumber % 2 == number % 2) return RIGHT_SIDE;
  if (_leftStartNumber % 2 == number % 2) return LEFT_SIDE;
  if (_leftEndNumber % 2 == number % 2) return LEFT_SIDE;
  
  return UNKNOWN_SIDE;	
}




bool 
RoadChunk::hasNumber (AddressNumber number) const
{
  AddressNumberSide ans = getSideForNumber(number);
  
  if (ans == RIGHT_SIDE) {
    
    if (getRightStartNumber() == ADDRESS_NUMBER_UNKNOWN) return false;
    if (getRightEndNumber() == ADDRESS_NUMBER_UNKNOWN) return false;

    if ((getRightStartNumber() <= number) &&
	(getRightEndNumber() >= number)) return true; 
    
  } else if (ans == LEFT_SIDE) {
    
    if (getLeftStartNumber() == ADDRESS_NUMBER_UNKNOWN) return false;
    if (getLeftEndNumber() == ADDRESS_NUMBER_UNKNOWN) return false;

    if ((getLeftStartNumber() <= number) &&
	(getLeftEndNumber() >= number)) return true; 
    
  }
  return false;
}





}
