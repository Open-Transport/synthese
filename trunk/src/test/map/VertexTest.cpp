#include "VertexTest.h"

#include <iostream>
#include <algorithm>
#include <set>

#include "Vertex.h"
#include "Topography.h"


using namespace std;



namespace synmap
{




  void 
  VertexTest::setUp () 
  {

  }


  void 
  VertexTest::tearDown() 
  {

  } 



  void
  VertexTest::testFindingCloseNeighbors ()
  {
    Topography topo;
    const Vertex* A = topo.newVertex (1.0, 1.0);
    const Vertex* B = topo.newVertex (4.0, 2.0);
    const Vertex* C = topo.newVertex (6.0, 3.0);
    const Vertex* D = topo.newVertex (4.0, 5.0);
    const Vertex* E = topo.newVertex (5.0, 6.0);
    const Vertex* F = topo.newVertex (7.0, 9.0);
    const Vertex* G = topo.newVertex (12.0, 8.0);
    const Vertex* H = topo.newVertex (8.0, 2.0);
    const Vertex* I = topo.newVertex (13.0, 2.0);
    const Vertex* J = topo.newVertex (15.0, 5.0);

    topo.newEdge (C, B); topo.newEdge (B, A);
    topo.newEdge (C, D); topo.newEdge (D, E);
    topo.newEdge (C, F);
    topo.newEdge (C, G);
    topo.newEdge (C, J);
    topo.newEdge (C, H); topo.newEdge (H, I);

    std::set<const Vertex*> neighbors =
      C->findCloseNeighbors (4.0);

    CPPUNIT_ASSERT_EQUAL (3, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());

    neighbors =
      C->findCloseNeighbors (6.0);

    CPPUNIT_ASSERT_EQUAL (5, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (A) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (E) != neighbors.end ());

    neighbors =
      C->findCloseNeighbors (20.0);

    CPPUNIT_ASSERT_EQUAL (9, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (A) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (E) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (F) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (G) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (I) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (J) != neighbors.end ());


  }


  void
  VertexTest::testFindingCloseNeighborsWithLoops ()
  {
    Topography topo;
    const Vertex* A = topo.newVertex (1.0, 1.0);
    const Vertex* B = topo.newVertex (4.0, 2.0);
    const Vertex* C = topo.newVertex (6.0, 3.0);
    const Vertex* D = topo.newVertex (4.0, 5.0);
    const Vertex* E = topo.newVertex (5.0, 6.0);
    const Vertex* F = topo.newVertex (7.0, 9.0);
    const Vertex* G = topo.newVertex (12.0, 8.0);
    const Vertex* H = topo.newVertex (8.0, 2.0);
    const Vertex* I = topo.newVertex (13.0, 2.0);
    const Vertex* J = topo.newVertex (15.0, 5.0);

    topo.newEdge (C, B); topo.newEdge (B, A);
    topo.newEdge (B, C); topo.newEdge (A, B);

    topo.newEdge (C, D); topo.newEdge (D, E);
    topo.newEdge (D, C); topo.newEdge (E, D);

    topo.newEdge (C, F);
    topo.newEdge (F, C);

    topo.newEdge (C, G);
    topo.newEdge (G, C);

    topo.newEdge (C, J);
    topo.newEdge (J, C);

    topo.newEdge (C, H); topo.newEdge (H, I);
    topo.newEdge (H, C); topo.newEdge (I, H);

    std::set<const Vertex*> neighbors =
      C->findCloseNeighbors (4.0);

    CPPUNIT_ASSERT_EQUAL (3, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());

    neighbors =
      C->findCloseNeighbors (6.0);

    CPPUNIT_ASSERT_EQUAL (5, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (A) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (E) != neighbors.end ());

    neighbors =
      C->findCloseNeighbors (20.0);

    CPPUNIT_ASSERT_EQUAL (9, (int) neighbors.size ());
    CPPUNIT_ASSERT (neighbors.find (B) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (D) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (H) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (A) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (E) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (F) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (G) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (I) != neighbors.end ());
    CPPUNIT_ASSERT (neighbors.find (J) != neighbors.end ());


  }


  void
  VertexTest::testFindingPathsToCloseNeighbors ()
  {
    Topography topo;
    const Vertex* A = topo.newVertex (1.0, 1.0);
    const Vertex* B = topo.newVertex (4.0, 2.0);
    const Vertex* C = topo.newVertex (6.0, 3.0);
    const Vertex* D = topo.newVertex (4.0, 5.0);
    const Vertex* E = topo.newVertex (5.0, 6.0);
    const Vertex* F = topo.newVertex (7.0, 9.0);
    const Vertex* G = topo.newVertex (12.0, 8.0);
    const Vertex* H = topo.newVertex (8.0, 2.0);
    const Vertex* I = topo.newVertex (13.0, 2.0);
    const Vertex* J = topo.newVertex (15.0, 5.0);

    topo.newEdge (C, B); topo.newEdge (B, A);
    topo.newEdge (C, D); topo.newEdge (D, E);
    topo.newEdge (C, F);
    topo.newEdge (C, G);
    topo.newEdge (C, J);
    topo.newEdge (C, H); topo.newEdge (H, I);

    std::set< std::vector<const Vertex*> > paths =
      C->findPathsToCloseNeighbors (4.0);
    
    CPPUNIT_ASSERT_EQUAL (3, (int) paths.size ());

    std::vector<const Vertex*> expected;
    expected.push_back (C);
    expected.push_back (B);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());
    expected.clear();
    expected.push_back (C);
    expected.push_back (D);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());
    expected.clear();
    expected.push_back (C);
    expected.push_back (H);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());

    paths = C->findPathsToCloseNeighbors (20.0);

    // Only the longest paths are preserved in the result list
    CPPUNIT_ASSERT_EQUAL (6, (int) paths.size ());
  }


  void
  VertexTest::testFindingPathsToCloseNeighborsWithLoops ()
  {
    Topography topo;
    const Vertex* A = topo.newVertex (1.0, 1.0);
    const Vertex* B = topo.newVertex (4.0, 2.0);
    const Vertex* C = topo.newVertex (6.0, 3.0);
    const Vertex* D = topo.newVertex (4.0, 5.0);
    const Vertex* E = topo.newVertex (5.0, 6.0);
    const Vertex* F = topo.newVertex (7.0, 9.0);
    const Vertex* G = topo.newVertex (12.0, 8.0);
    const Vertex* H = topo.newVertex (8.0, 2.0);
    const Vertex* I = topo.newVertex (13.0, 2.0);
    const Vertex* J = topo.newVertex (15.0, 5.0);

    topo.newEdge (C, B); topo.newEdge (B, A);
    topo.newEdge (B, C); topo.newEdge (A, B);
    topo.newEdge (C, D); topo.newEdge (D, E);
    topo.newEdge (D, C); topo.newEdge (E, D);
    topo.newEdge (C, F);
    topo.newEdge (F, C);
    topo.newEdge (C, G);
    topo.newEdge (G, C);
    topo.newEdge (C, J);
    topo.newEdge (J, C);
    topo.newEdge (C, H); topo.newEdge (H, I);
    topo.newEdge (H, C); topo.newEdge (I, H);

    std::set< std::vector<const Vertex*> > paths =
      C->findPathsToCloseNeighbors (4.0);
    
    CPPUNIT_ASSERT_EQUAL (3, (int) paths.size ());

    std::vector<const Vertex*> expected;
    expected.push_back (C);
    expected.push_back (B);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());
    expected.clear();
    expected.push_back (C);
    expected.push_back (D);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());
    expected.clear();
    expected.push_back (C);
    expected.push_back (H);
    CPPUNIT_ASSERT (paths.find(expected) != paths.end ());

  }


}
