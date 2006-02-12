#include "RoadChunk.h"

#include <cmath>
#include "Address.h"


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
    , _rightStartNumber (rightStartNumber)
    , _rightEndNumber (rightEndNumber)
    , _leftStartNumber (leftStartNumber)
    , _leftEndNumber (leftEndNumber)
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





}
