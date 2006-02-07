#include "RoadChunkTest.h"

#include <iostream>

#include "RoadChunk.h"
#include "Topography.h"



namespace synmap
{




  void 
  RoadChunkTest::setUp () 
  {

  }


  void 
  RoadChunkTest::tearDown() 
  {

  } 


  void
  RoadChunkTest::testConstruction ()
  {
    Topography topo;
    std::vector<const Location*> steps;
    
    RoadChunk::AddressNumber rightStartNumber = 3;
    RoadChunk::AddressNumber rightEndNumber = 21;
    RoadChunk::AddressNumber leftStartNumber = 4;
    RoadChunk::AddressNumber leftEndNumber = 24;

    const RoadChunk* chunk = topo.newRoadChunk (0, steps,
						rightStartNumber, rightEndNumber,
						leftStartNumber, leftEndNumber);

    CPPUNIT_ASSERT_EQUAL (rightStartNumber, chunk->getRightStartNumber ());
    CPPUNIT_ASSERT_EQUAL (rightEndNumber, chunk->getRightEndNumber ());
    CPPUNIT_ASSERT_EQUAL (leftStartNumber, chunk->getLeftStartNumber ());
    CPPUNIT_ASSERT_EQUAL (leftEndNumber, chunk->getLeftEndNumber ());
    

  }
    



  void
  RoadChunkTest::testAddressNumbers1 ()
  {
    // The start end address numbers are known on both sides
    
    Topography topo;
    std::vector<const Location*> steps;
    
    RoadChunk::AddressNumber rightStartNumber = 3;
    RoadChunk::AddressNumber rightEndNumber = 21;
    RoadChunk::AddressNumber leftStartNumber = 4;
    RoadChunk::AddressNumber leftEndNumber = 24;

    const RoadChunk* chunk = topo.newRoadChunk (0, steps,
						rightStartNumber, rightEndNumber,
						leftStartNumber, leftEndNumber);

    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (25));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (23));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (2));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (1));

    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (3));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (4));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (21));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (24));
    
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (5));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (22));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (10));
    

  }



  void
  RoadChunkTest::testAddressNumbers2 ()
  {
    // The start end address numbers are known on one side only
    
    Topography topo;
    std::vector<const Location*> steps;
    
    RoadChunk::AddressNumber rightStartNumber = 3;
    RoadChunk::AddressNumber rightEndNumber = 21;
    RoadChunk::AddressNumber leftStartNumber = RoadChunk::ADDRESS_NUMBER_UNKNOWN;
    RoadChunk::AddressNumber leftEndNumber = RoadChunk::ADDRESS_NUMBER_UNKNOWN;

    const RoadChunk* chunk = topo.newRoadChunk (0, steps,
						rightStartNumber, rightEndNumber,
						leftStartNumber, leftEndNumber);

    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (25));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (23));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (2));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (1));

    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (3));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (4));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (21));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (24));
    
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (5));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (22));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (10));
    

  }


  void
  RoadChunkTest::testAddressNumbers3 ()
  {
    // The start end address numbers are known on one side only
    // On the other side only end number is known
    
    Topography topo;
    std::vector<const Location*> steps;
    
    RoadChunk::AddressNumber rightStartNumber = 3;
    RoadChunk::AddressNumber rightEndNumber = 21;
    RoadChunk::AddressNumber leftStartNumber = RoadChunk::ADDRESS_NUMBER_UNKNOWN;
    RoadChunk::AddressNumber leftEndNumber = 28;

    const RoadChunk* chunk = topo.newRoadChunk (0, steps,
						rightStartNumber, rightEndNumber,
						leftStartNumber, leftEndNumber);

    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (25));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (23));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (2));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (1));

    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (3));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (4));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (21));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (24));
    
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (5));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (22));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (10));
    

  }

  void
  RoadChunkTest::testAddressNumbers4 ()
  {
    // Ambiguities on both sides
    
    Topography topo;
    std::vector<const Location*> steps;
    
    RoadChunk::AddressNumber rightStartNumber = 3;
    RoadChunk::AddressNumber rightEndNumber = RoadChunk::ADDRESS_NUMBER_UNKNOWN;
    RoadChunk::AddressNumber leftStartNumber = RoadChunk::ADDRESS_NUMBER_UNKNOWN;
    RoadChunk::AddressNumber leftEndNumber = 28;

    const RoadChunk* chunk = topo.newRoadChunk (0, steps,
						rightStartNumber, rightEndNumber,
						leftStartNumber, leftEndNumber);

    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (3));
    CPPUNIT_ASSERT_EQUAL (true, chunk->hasNumber (28));

    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (25));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (23));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (2));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (1));

    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (4));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (21));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (24));
    
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (5));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (22));
    CPPUNIT_ASSERT_EQUAL (false, chunk->hasNumber (10));
    

  }







}
