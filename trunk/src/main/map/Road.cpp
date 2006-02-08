#include "Road.h"
#include "RoadChunk.h"
#include "Location.h"
#include "Vertex.h"
#include "PhysicalStop.h"
#include "Topography.h"


#include <cmath>
#include <assert.h>


namespace synmap
{



Road::Road(Topography* topography,
	   int key,
	   const std::string& name,
	   const std::string& discriminant,
	   const std::vector<RoadChunk*>& chunks)
  : Referrant (topography, key)
    , _name (name)
    , _discriminant (discriminant)
{
  for (std::vector<RoadChunk*>::const_iterator iter (chunks.begin ());
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
Road::findMostPlausibleChunkForNumber (const RoadChunk::AddressNumber& number) const
{
  // Warning : this algorithm works only assuming that chunks are ordered!

  RoadChunk::AddressNumber closestMin = std::numeric_limits<double>::min ();
  int closestMinChunk = -1;

  RoadChunk::AddressNumber closestMax = std::numeric_limits<double>::max ();
  int closestMaxChunk = -1;

  RoadChunk::AddressNumber unknown = RoadChunk::ADDRESS_NUMBER_UNKNOWN;

  int index = 0;

  /*  for (std::vector<const RoadChunk*>::const_iterator iter (_chunks.begin ());
      iter != _chunks.end (); ++iter) { */
  for (int i=0; i<_chunks.size (); ++i) {

    const RoadChunk* chunk = _chunks[i];
    // const RoadChunk* chunk = *iter;
    if (chunk->hasNumber (number)) return chunk;

    
    RoadChunk::AddressNumber rsn = chunk->getRightStartNumber ();
    RoadChunk::AddressNumber lsn = chunk->getLeftStartNumber ();
    if ((rsn != unknown) && (rsn < number) && rsn > closestMin) {
      closestMin = rsn;
      closestMinChunk = index;
    } else if ((lsn != unknown) && (lsn < number) && lsn > closestMin) {
      closestMin = lsn;
      closestMinChunk = index;
    }

    RoadChunk::AddressNumber ren = chunk->getRightEndNumber ();
    RoadChunk::AddressNumber len = chunk->getLeftEndNumber ();
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




std::set< Road::PathToPhysicalStop >
Road::findPathsToPhysicalStops (RoadChunk::AddressNumber addressNumber, 
				double distance) {
  const RoadChunk* chunk = findMostPlausibleChunkForNumber (addressNumber);
  // TODO : add an algorithm to find more precisely the vertex to
  // start from ?
  const Vertex* start = chunk->getStep (0)->getVertex ();
  std::set< std::vector<const Vertex*> > paths = 
    start->findPathsToCloseNeighbors (distance);

  std::set< Road::PathToPhysicalStop > result;

  RoadChunkVector tmpChunks;

  for (std::set< std::vector<const Vertex*> >::iterator path = paths.begin ();
       path != paths.end ();
       ++path) {
    
    for (int i=0; i<path->size (); ++i) {
      const Vertex* v = path->at (i);
      const PhysicalStop* pstop = dynamic_cast<const PhysicalStop*> (v);
      if (pstop != 0) {
	// Create an entry in result which is a path from start
	// to the found physical stop
	RoadChunkVector pathChunks;

	for (int j=0; j+1<=i; ++j) {
	  const Edge* edge = getTopography()->getEdge (path->at (j), path->at (j+1));
	  
	  // Normally, there can be only one road chunk associated
	  // with an edge.
	  getTopography ()->findRoadChunks (edge, tmpChunks);
	  assert (tmpChunks.size () == 1);
	  pathChunks.push_back (tmpChunks.at (0));
	}

	result.insert (PathToPhysicalStop (pathChunks, pstop));
	
      }

    }
    return result;

  }
  

  


  


} 



/*

std::set<const PhysicalStop*> 
Road::findClosePhysicalStops (RoadChunk::AddressNumber addressNumber, double distance) const
{
  const RoadChunk* chunk = findMostPlausibleChunkForNumber (addressNumber);
  
  // Start on the first vertex
  // TODO : add an algorithm to find more precisely the vertex to
  // start from

  const Vertex* start = chunk->getStep (0);
  
  std::set<const Vertex*> walkedVertices;
  
  
  // Walk all vertices from start not passing 2 times through
  // the same vertex. 
  // As an output we expect a list of all path measuring less than
  // distance








1. find path to vertices within the given distance
2. go through paths and create a separate path for each physical stop encountered
   filtering those who contains none
3. for each reamining path, take edges one by one and add the corresponding roadchunk


}
  */


}
