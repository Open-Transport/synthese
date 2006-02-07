#include "RoadTest.h"

#include <iostream>

#include "RoadChunk.h"
#include "Road.h"
#include "Topography.h"



namespace synmap
{




  void 
  RoadTest::setUp () 
  {

  }


  void 
  RoadTest::tearDown() 
  {

  } 



  void
  RoadTest::testMostPlausibleAddressNumber1 ()
  {
    // R 1      ? 7           21 23    31 ?       ?
    //   <======> <============> <======> <=======>
    // L 2      ? ?           24 26    32 ?       ?

    Topography topo;
    std::vector<const Location*> steps;
    std::vector<RoadChunk*> chunks;
    RoadChunk::AddressNumber unknown =  RoadChunk::ADDRESS_NUMBER_UNKNOWN;

    RoadChunk* chunk1 = topo.newRoadChunk (0, steps, 1, unknown, 2, unknown);
    RoadChunk* chunk2 = topo.newRoadChunk (1, steps, 7, 21, unknown, 24);
    RoadChunk* chunk3 = topo.newRoadChunk (2, steps, 23, 31, 26, 32);
    RoadChunk* chunk4 = topo.newRoadChunk (3, steps, unknown, unknown, unknown, unknown);
    
    chunks.push_back (chunk1);
    chunks.push_back (chunk2);
    chunks.push_back (chunk3);
    chunks.push_back (chunk4);

    const Road* road = topo.newRoad (0, "name", "dicr", chunks);
    
    // If one of the chunk has the number unambiguously, return it.
    CPPUNIT_ASSERT (chunk2 == road->findMostPlausibleChunkForNumber (9));
    CPPUNIT_ASSERT (chunk3 == road->findMostPlausibleChunkForNumber (28));

    // Otherwise...
    CPPUNIT_ASSERT (chunk1 == road->findMostPlausibleChunkForNumber (5));
    CPPUNIT_ASSERT (chunk1 == road->findMostPlausibleChunkForNumber (4));
    CPPUNIT_ASSERT (chunk2 == road->findMostPlausibleChunkForNumber (22));

    CPPUNIT_ASSERT (chunk4 == road->findMostPlausibleChunkForNumber (34));
    CPPUNIT_ASSERT (chunk4 == road->findMostPlausibleChunkForNumber (37));

  }


}
