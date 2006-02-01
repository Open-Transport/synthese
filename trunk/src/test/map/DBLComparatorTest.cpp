#include "DBLComparatorTest.h"

#include <iostream>

#include "DBLComparator.h"
#include "Map.h"
#include "Vertex.h"
#include "Itinerary.h"
#include "Rectangle.h"
#include "DrawableBusLine.h"
#include "PostscriptCanvas.h"
#include "Topography.h"



namespace synmap
{




  void 
  DBLComparatorTest::setUp () 
  {

  }


  void 
  DBLComparatorTest::tearDown() 
  {

  } 



  void 
  DBLComparatorTest::testVariousComparisons () {
    Topography topo;
    int iti_key = 0;
    const Vertex* A = topo.newVertex (2.0, 3.0);
    const Vertex* B = topo.newVertex (6.0, 6.0);
    const Vertex* C = topo.newVertex (9.0, 5.0);
    const Vertex* D = topo.newVertex (12.0, 9.0);
    const Vertex* E = topo.newVertex (12.0, 13.0);
    const Vertex* F = topo.newVertex (15.0, 15.0);
    const Vertex* G = topo.newVertex (18.0, 18.0);
    const Vertex* H = topo.newVertex (20.0, 20.0);
    const Vertex* I = topo.newVertex (18.0, 23.0);

    const Vertex* J = topo.newVertex (8.0, 14.0);
    const Vertex* K = topo.newVertex (16.0, 22.0);

    const Vertex* L = topo.newVertex (16.0, 7.0);
    const Vertex* M = topo.newVertex (26.0, 20.0);
    const Vertex* N = topo.newVertex (19.0, 8.0);

    const Vertex* O = topo.newVertex (12.0, 1.0);
    const Vertex* P = topo.newVertex (14.0, 9.0);

    const Vertex* Q = topo.newVertex (29.0, 14.0);

    const Vertex* R = topo.newVertex (10.0, 20.0);
    const Vertex* S = topo.newVertex (20.0, 10.0);


    std::vector<const Vertex*> vertices;
    std::vector<bool> pstops;

    vertices.push_back (A); pstops.push_back (false);
    vertices.push_back (B); pstops.push_back (false);
    vertices.push_back (C); pstops.push_back (false);
    vertices.push_back (D); pstops.push_back (false);
    vertices.push_back (E); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (H); pstops.push_back (false);
    vertices.push_back (I); pstops.push_back (false);
	
    Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti1->setProperty("name", "bus1");
    iti1->setProperty("lineNumber", "1");
    iti1->setProperty("color", "red");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (J); pstops.push_back (false);
    vertices.push_back (E); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (K); pstops.push_back (false);

    Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti2->setProperty("name", "bus2");
    iti2->setProperty("lineNumber", "2");
    iti2->setProperty("color", "blue");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (M); pstops.push_back (false);
    vertices.push_back (H); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (L); pstops.push_back (false);

    Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti3->setProperty("name", "bus3");
    iti3->setProperty("lineNumber", "3");
    iti3->setProperty("color", "cyan");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (H); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (N); pstops.push_back (false);

    Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti4->setProperty("name", "bus4");
    iti4->setProperty("lineNumber", "4");
    iti4->setProperty("color", "yellow");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (H); pstops.push_back (false);

    Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti5->setProperty("name", "bus5");
    iti5->setProperty("lineNumber", "5");
    iti5->setProperty("color", "green");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (K); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (E); pstops.push_back (false);
    vertices.push_back (J); pstops.push_back (false);

    Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti6->setProperty("name", "bus6");
    iti6->setProperty("lineNumber", "6");
    iti6->setProperty("color", "magenta");
	
    vertices.clear ();
    pstops.clear ();

    vertices.push_back (L); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (H); pstops.push_back (false);
    vertices.push_back (M); pstops.push_back (false);

    Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti7->setProperty("name", "bus7");
    iti7->setProperty("lineNumber", "7");
    iti7->setProperty("color", "blue");

    vertices.clear ();
    pstops.clear ();
    vertices.push_back (O); pstops.push_back (false);
    vertices.push_back (C); pstops.push_back (false);
    vertices.push_back (D); pstops.push_back (false);
    vertices.push_back (P); pstops.push_back (false);

    Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti8->setProperty("name", "bus8");
    iti8->setProperty("lineNumber", "8");
    iti8->setProperty("color", "green");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (P); pstops.push_back (false);
    vertices.push_back (D); pstops.push_back (false);
    vertices.push_back (C); pstops.push_back (false);
    vertices.push_back (O); pstops.push_back (false);

    Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti9->setProperty("name", "bus9");
    iti9->setProperty("lineNumber", "9");
    iti9->setProperty("color", "yellow");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (H); pstops.push_back (false);
    vertices.push_back (G); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (Q); pstops.push_back (false);

    Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti10->setProperty("name", "bus10");
    iti10->setProperty("lineNumber", "10");
    iti10->setProperty("color", "red");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (R); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (S); pstops.push_back (false);

    DrawableBusLine* dbl1 = new DrawableBusLine (iti1);
    DrawableBusLine* dbl2 = new DrawableBusLine (iti2);
    DrawableBusLine* dbl3 = new DrawableBusLine (iti3);
    DrawableBusLine* dbl4 = new DrawableBusLine (iti4);
    DrawableBusLine* dbl5 = new DrawableBusLine (iti5);
    DrawableBusLine* dbl6 = new DrawableBusLine (iti6);
	
	
    {
      DBLComparator cmp (dbl1, F, F);
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl2, dbl1)); // Following 1, 2 is on the left of 1
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl2)); // Following 1, 1 is on the right of 2
    }
    {
      DBLComparator cmp (dbl2, F, F);
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl2, dbl1)); // Following 2, 2 is on the left of 1
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl2)); // Following 2, 1 is on the right of 2
    }
    {
      DBLComparator cmp (dbl3, H, H);
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl4)); // Following 3, 3 is on the right of 4
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl4, dbl3)); // Following 3, 4 is on the left of 3
    }
    {
      DBLComparator cmp (dbl4, H, H);
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl4)); // Following 4, 3 is on the right of 4
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl4, dbl3)); // Following 4, 4 is on the left of 3
    }
    {
      DBLComparator cmp (dbl1, H, H);
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl3, dbl4)); // Following 1, 3 is on the left of 4
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl4, dbl3)); // Following 1, 4 is on the right of 3
    }
    {
      DBLComparator cmp (dbl1, F, F);
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl1, dbl5)); // Following 1, 1 is on the left of 5
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl5, dbl1)); // Following 1, 5 is on the right of 1
    }
    {
      DBLComparator cmp (dbl1, F, F);
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl5)); // Following 1, 3 is on the right of 5
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl5, dbl3)); // Following 1, 5 is on the left of 1
    }
    {
      DBLComparator cmp (dbl1, F, F);
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl6)); // Following 1, 1 is on the right of 6
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl6, dbl1)); // Following 1, 6 is on the left of 1
    }
    {
      DBLComparator cmp (dbl5, F, F);
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl3, dbl4)); // Following 5, 3 is on the left of 4
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl4, dbl3)); // Following 5, 4 is on the right of 3
    }
    {
      DBLComparator cmp (dbl6, F, F);
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl6, dbl5)); // Following 6, 6 is on the right of 5
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl5, dbl6)); // Following 6, 5 is on the left of 6
    }
	
		
	
    // Finally dump a map
    /* std::ofstream of ("/home/mjambert/map2.ps");
    Map map (&topo, of, Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
    map.dump();*/
	
  }





}
