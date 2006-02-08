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
  RoadTest::testMostPlausibleAddressNumber ()
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

    const Road* road = topo.newRoad (0, "name", "dicr", 0, chunks);
    
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

  void
  RoadTest::testFindingPathsToPhysicalStops () {
    /*

    S = Start
    + = Chunk start/end
    # = physical stop

    A         B        C        D        E
    +----S----+--------+--------+----#---+  <- Road1
                       |             ps1
                       # ps2
                       |        G        H
                     F +--------+----#---+  <- Road2
                       |        |    ps3
                       |        |
                       |        |
                     I +      J +
                                |
                Road3 -^        |
                                |
                              K +
                        
                                ^- Road4
    */

    
    Topography topo;
    const Vertex* A = topo.newVertex (0.0, 8.0);
    const Vertex* vS = topo.newVertex (1.0, 8.0);
    const Vertex* B = topo.newVertex (2.0, 8.0);
    const Vertex* C = topo.newVertex (4.0, 8.0);
    const Vertex* D = topo.newVertex (6.0, 8.0);
    const Vertex* vPs1 = topo.newVertex (7.0, 8.0);
    const Vertex* E = topo.newVertex (8.0, 8.0);
    const Vertex* F = topo.newVertex (4.0, 6.0);
    const Vertex* vPs2 = topo.newVertex (4.0, 7.0);
    const Vertex* G = topo.newVertex (6.0, 6.0);
    const Vertex* vPs3 = topo.newVertex (7.0, 6.0);
    const Vertex* H = topo.newVertex (8.0, 6.0);
    const Vertex* I = topo.newVertex (4.0, 4.0);
    const Vertex* J = topo.newVertex (6.0, 4.0);
    const Vertex* K = topo.newVertex (6.0, 2.0);

    const PhysicalStop* ps1 = topo.newPhysicalStop (1, vPs1);
    const PhysicalStop* ps2 = topo.newPhysicalStop (2, vPs2);
    const PhysicalStop* ps3 = topo.newPhysicalStop (3, vPs3);







  }





}
