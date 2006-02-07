#include "RoadChunk.h"

#include <cmath>


namespace synmap
{

const RoadChunk::AddressNumber RoadChunk::ADDRESS_NUMBER_UNKNOWN = 0.0;


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
  int n = (int) std::floor (number);
  int rsn = (int) std::floor (_rightStartNumber);
  int ren = (int) std::floor (_rightEndNumber);
  int lsn = (int) std::floor (_leftStartNumber);
  int len = (int) std::floor (_leftEndNumber);


  if (rsn % 2 == n % 2) return RIGHT_SIDE;
  if (ren % 2 == n % 2) return RIGHT_SIDE;
  if (lsn % 2 == n % 2) return LEFT_SIDE;
  if (len % 2 == n % 2) return LEFT_SIDE;
  
  return UNKNOWN_SIDE;	
}




bool 
RoadChunk::hasNumber (AddressNumber number) const
{
  AddressNumberSide ans = getSideForNumber(number);

  if (getRightStartNumber() == number) return true;
  if (getRightEndNumber() == number) return true;
  if (getLeftStartNumber() == number) return true;
  if (getLeftEndNumber() == number) return true;
  
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
