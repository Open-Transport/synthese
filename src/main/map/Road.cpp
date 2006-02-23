#include "Road.h"
#include "RoadChunk.h"
#include "Location.h"
#include "Vertex.h"
#include "Edge.h"
#include "PhysicalStop.h"
#include "Topography.h"


#include <cmath>
#include <assert.h>

#include <iostream>

#include <limits>

using namespace std;


namespace synmap
{



Road::Road(Topography* topography,
	   int key,
	   const std::string& name,
	   const RoadType& type,
	   const std::string& discriminant,
	   cCommune* town,
	   const std::vector<RoadChunk*>& chunks)
  : Referrant (topography, key)
  , _name (name)
  , _type (type)
  , _discriminant (discriminant)
  , _town (town)
{
  for(std::vector<RoadChunk*>::const_iterator iter (chunks.begin ());
       iter != chunks.end (); ++iter) {
    RoadChunk* chunk = *iter;
    chunk->setRoad (this);
    _chunks.push_back (chunk);
  }

}



Road::~Road()
{
}




const RoadChunk* 
Road::findMostPlausibleChunkForNumber (const Address::AddressNumber& number) const
{
  // Warning : this algorithm works only assuming that chunks are ordered!

  Address::AddressNumber closestMin = std::numeric_limits<double>::min ();
  int closestMinChunk = -1;

  Address::AddressNumber closestMax = std::numeric_limits<double>::max ();
  int closestMaxChunk = -1;

  Address::AddressNumber unknown = Address::UNKNOWN_ADDRESS_NUMBER;

  int index = 0;

  /*  for (std::vector<const RoadChunk*>::const_iterator iter (_chunks.begin ());
      iter != _chunks.end (); ++iter) { */
  for (int i=0; i<_chunks.size (); ++i) {

    const RoadChunk* chunk = _chunks[i];
    // const RoadChunk* chunk = *iter;
    if (chunk->hasNumber (number)) return chunk;

    
    Address::AddressNumber rsn = chunk->getRightStartNumber ();
    Address::AddressNumber lsn = chunk->getLeftStartNumber ();
    if ((rsn != unknown) && (rsn < number) && rsn > closestMin) {
      closestMin = rsn;
      closestMinChunk = index;
    } else if ((lsn != unknown) && (lsn < number) && lsn > closestMin) {
      closestMin = lsn;
      closestMinChunk = index;
    }

    Address::AddressNumber ren = chunk->getRightEndNumber ();
    Address::AddressNumber len = chunk->getLeftEndNumber ();
    if ((ren != unknown) && (ren > number) && ren < closestMax) {
      closestMax = ren;
      closestMaxChunk = index;
    } else if ((len != unknown) && (len > number) && len < closestMax) {
      closestMax = len;
      closestMaxChunk = index;
    }
    ++index;
  }


  // if closestMin and closestMax were assigned
  if ((closestMinChunk != -1) && (closestMaxChunk != -1)) {
    double nbAmbiguousChunks = closestMaxChunk - closestMinChunk;
    double nbAmbiguousNumbers = closestMax - closestMin;
    double nbAmbiguousNumbersPerChunk = nbAmbiguousNumbers / nbAmbiguousChunks;
    int plausibleIndex = (int) std::floor (closestMinChunk + (number - closestMin) / nbAmbiguousNumbersPerChunk);
    return _chunks[plausibleIndex];
  } 

  // only closestMin was assigned
  if (closestMinChunk != -1) {
    if (closestMinChunk < _chunks.size ()-1) {
      return _chunks[closestMinChunk+1];
    } else {
      return _chunks[closestMinChunk];
    }
  }

  // only closestMax was assigned
  if (closestMaxChunk != -1) {
    if (closestMaxChunk > 0) {
      return _chunks[closestMaxChunk-1];
    } else {
      return _chunks[closestMaxChunk];
    }
  }

  // Really no useful info, return the first chunk...
  return _chunks[0];

}









}
