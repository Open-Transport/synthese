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
    const Vertex* B = topo.newVertex (2.0, 8.0);
    const Vertex* C = topo.newVertex (4.0, 8.0);
    const Vertex* D = topo.newVertex (6.0, 8.0);
    const Vertex* E = topo.newVertex (8.0, 8.0);
    const Vertex* F = topo.newVertex (4.0, 6.0);
    const Vertex* G = topo.newVertex (6.0, 6.0);
    const Vertex* H = topo.newVertex (8.0, 6.0);
    const Vertex* I = topo.newVertex (4.0, 4.0);
    const Vertex* J = topo.newVertex (6.0, 4.0);
    const Vertex* K = topo.newVertex (6.0, 2.0);

    const Vertex* vPs1 = topo.newVertex (7.0, 8.0);
    const Vertex* vPs2 = topo.newVertex (4.0, 7.0);
    const Vertex* vPs3 = topo.newVertex (7.0, 6.0);

    const PhysicalStop* ps1 = topo.newPhysicalStop (1, vPs1);
    const PhysicalStop* ps2 = topo.newPhysicalStop (2, vPs2);
    const PhysicalStop* ps3 = topo.newPhysicalStop (3, vPs3);

    // Road chunks creation
    int chunkKey = 0;
    std::vector<const Location*> steps;
    steps.push_back (topo.newLocation (A));
    steps.push_back (topo.newLocation (B));
    RoadChunk* chunkAB = topo.newRoadChunk (chunkKey++, steps, 1, 7, 2, 8);

    steps.clear ();
    steps.push_back (topo.newLocation (B));
    steps.push_back (topo.newLocation (C));
    RoadChunk* chunkBC = topo.newRoadChunk (chunkKey++, steps, 9, 15, 10, 16);

    steps.clear ();
    steps.push_back (topo.newLocation (C));
    steps.push_back (topo.newLocation (D));
    RoadChunk* chunkCD = topo.newRoadChunk (chunkKey++, steps, 17, 23, 18, 24);

    steps.clear ();
    steps.push_back (topo.newLocation (D));
    steps.push_back (topo.newLocation (vPs1));
    steps.push_back (topo.newLocation (E));
    RoadChunk* chunkDps1E = topo.newRoadChunk (chunkKey++, steps, 25, 31, 26, 32);

    steps.clear ();
    steps.push_back (topo.newLocation (C));
    steps.push_back (topo.newLocation (vPs2));
    steps.push_back (topo.newLocation (F));
    RoadChunk* chunkCps2F = topo.newRoadChunk (chunkKey++, steps, 1, 7, 2, 8);

    steps.clear ();
    steps.push_back (topo.newLocation (F));
    steps.push_back (topo.newLocation (I));
    RoadChunk* chunkFI = topo.newRoadChunk (chunkKey++, steps, 9, 15, 10, 16);

    steps.clear ();
    steps.push_back (topo.newLocation (F));
    steps.push_back (topo.newLocation (G));
    RoadChunk* chunkFG = topo.newRoadChunk (chunkKey++, steps, 1, 7, 2, 8);

    steps.clear ();
    steps.push_back (topo.newLocation (G));
    steps.push_back (topo.newLocation (vPs3));
    steps.push_back (topo.newLocation (H));
    RoadChunk* chunkGps3H = topo.newRoadChunk (chunkKey++, steps, 9, 15, 10, 16);

    steps.clear ();
    steps.push_back (topo.newLocation (G));
    steps.push_back (topo.newLocation (J));
    RoadChunk* chunkGJ = topo.newRoadChunk (chunkKey++, steps, 1, 7, 2, 8);

    steps.clear ();
    steps.push_back (topo.newLocation (J));
    steps.push_back (topo.newLocation (K));
    RoadChunk* chunkJK = topo.newRoadChunk (chunkKey++, steps, 9, 15, 10, 16);

    // Roads creation
    int roadKey = 0;
    std::vector<RoadChunk*> chunks;
    chunks.push_back (chunkAB);
    chunks.push_back (chunkBC);
    chunks.push_back (chunkCD);
    chunks.push_back (chunkDps1E);
    const Road* road1 = topo.newRoad (roadKey++, "road1", "disc", 0, chunks);

    chunks.clear ();
    chunks.push_back (chunkFG);
    chunks.push_back (chunkGps3H);
    const Road* road2 = topo.newRoad (roadKey++, "road2", "disc", 0, chunks);

    chunks.clear ();
    chunks.push_back (chunkCps2F);
    chunks.push_back (chunkFI);
    const Road* road3 = topo.newRoad (roadKey++, "road3", "disc", 0, chunks);

    chunks.clear ();
    chunks.push_back (chunkGJ);
    chunks.push_back (chunkJK);
    const Road* road4 = topo.newRoad (roadKey++, "road4", "disc", 0, chunks);

    
    std::set< Road::PathToPhysicalStop > paths = 
      road1->findPathsToPhysicalStops (4, 8.0);
    
    // only ps1 and ps2 are reachable
    CPPUNIT_ASSERT_EQUAL (2, (int) paths.size());

    Road::PathToPhysicalStop expectedPs1;
    expectedPs1.second = ps1;
    expectedPs1.first.push_back (chunkAB);
    expectedPs1.first.push_back (chunkBC);
    expectedPs1.first.push_back (chunkCD);
    expectedPs1.first.push_back (chunkDps1E);

    CPPUNIT_ASSERT (paths.find (expectedPs1) != paths.end ());
    
    Road::PathToPhysicalStop expectedPs2;
    expectedPs2.second = ps2;
    expectedPs2.first.push_back (chunkAB);
    expectedPs2.first.push_back (chunkBC);
    expectedPs2.first.push_back (chunkCps2F);

    CPPUNIT_ASSERT (paths.find (expectedPs2) != paths.end ());

    paths = road1->findPathsToPhysicalStops (4, 20.0);
    
    CPPUNIT_ASSERT_EQUAL (3, (int) paths.size());

    Road::PathToPhysicalStop expectedPs3;
    expectedPs3.second = ps3;
    expectedPs3.first.push_back (chunkAB);
    expectedPs3.first.push_back (chunkBC);
    expectedPs3.first.push_back (chunkCps2F);
    expectedPs3.first.push_back (chunkFG);
    expectedPs3.first.push_back (chunkGps3H);
    
    CPPUNIT_ASSERT (paths.find (expectedPs1) != paths.end ());
    CPPUNIT_ASSERT (paths.find (expectedPs2) != paths.end ());
    CPPUNIT_ASSERT (paths.find (expectedPs3) != paths.end ());


  }





}
