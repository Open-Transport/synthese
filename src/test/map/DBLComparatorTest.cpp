#include "DBLComparatorTest.h"

#include <iostream>

#include "DBLComparator.h"
#include "Map.h"
#include "Vertex.h"
#include "Location.h"
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
    const Location* A = topo.newLocation (2.0, 3.0);
    const Location* B = topo.newLocation (6.0, 6.0);
    const Location* C = topo.newLocation (9.0, 5.0);
    const Location* D = topo.newLocation (12.0, 9.0);
    const Location* E = topo.newLocation (12.0, 13.0);
    const Location* F = topo.newLocation (15.0, 15.0);
    const Location* G = topo.newLocation (18.0, 18.0);
    const Location* H = topo.newLocation (20.0, 20.0);
    const Location* I = topo.newLocation (18.0, 23.0);

    const Location* J = topo.newLocation (8.0, 14.0);
    const Location* K = topo.newLocation (16.0, 22.0);

    const Location* L = topo.newLocation (16.0, 7.0);
    const Location* M = topo.newLocation (26.0, 20.0);
    const Location* N = topo.newLocation (19.0, 8.0);

    const Location* O = topo.newLocation (12.0, 1.0);
    const Location* P = topo.newLocation (14.0, 9.0);

    const Location* Q = topo.newLocation (29.0, 14.0);

    const Location* R = topo.newLocation (10.0, 20.0);
    const Location* S = topo.newLocation (20.0, 10.0);


    std::vector<const Location*> locations;

    locations.push_back (A); 
    locations.push_back (B); 
    locations.push_back (C); 
    locations.push_back (D); 
    locations.push_back (E); 
    locations.push_back (F); 
    locations.push_back (G); 
    locations.push_back (H); 
    locations.push_back (I); 
	
    Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti1->setProperty("name", "bus1");
    iti1->setProperty("lineNumber", "1");
    iti1->setProperty("color", "red");
	
    locations.clear ();
    locations.push_back (J); 
    locations.push_back (E); 
    locations.push_back (F); 
    locations.push_back (G); 
    locations.push_back (K); 

    Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti2->setProperty("name", "bus2");
    iti2->setProperty("lineNumber", "2");
    iti2->setProperty("color", "blue");
	
    locations.clear ();
    locations.push_back (M); 
    locations.push_back (H); 
    locations.push_back (G); 
    locations.push_back (F); 
    locations.push_back (L); 

    Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti3->setProperty("name", "bus3");
    iti3->setProperty("lineNumber", "3");
    iti3->setProperty("color", "cyan");
	
    locations.clear ();
    locations.push_back (H); 
    locations.push_back (G); 
    locations.push_back (F); 
    locations.push_back (N); 

    Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti4->setProperty("name", "bus4");
    iti4->setProperty("lineNumber", "4");
    iti4->setProperty("color", "yellow");
	
    locations.clear ();
    locations.push_back (F); 
    locations.push_back (G); 
    locations.push_back (H); 

    Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti5->setProperty("name", "bus5");
    iti5->setProperty("lineNumber", "5");
    iti5->setProperty("color", "green");
	
    locations.clear ();
    locations.push_back (K); 
    locations.push_back (G); 
    locations.push_back (F); 
    locations.push_back (E); 
    locations.push_back (J); 

    Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti6->setProperty("name", "bus6");
    iti6->setProperty("lineNumber", "6");
    iti6->setProperty("color", "magenta");
	
    locations.clear ();

    locations.push_back (L); 
    locations.push_back (F); 
    locations.push_back (G); 
    locations.push_back (H); 
    locations.push_back (M); 

    Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti7->setProperty("name", "bus7");
    iti7->setProperty("lineNumber", "7");
    iti7->setProperty("color", "blue");

    locations.clear ();
    locations.push_back (O); 
    locations.push_back (C); 
    locations.push_back (D); 
    locations.push_back (P); 

    Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti8->setProperty("name", "bus8");
    iti8->setProperty("lineNumber", "8");
    iti8->setProperty("color", "green");
	
    locations.clear ();
    locations.push_back (P); 
    locations.push_back (D); 
    locations.push_back (C); 
    locations.push_back (O); 

    Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti9->setProperty("name", "bus9");
    iti9->setProperty("lineNumber", "9");
    iti9->setProperty("color", "yellow");
	
    locations.clear ();
    locations.push_back (H); 
    locations.push_back (G); 
    locations.push_back (F); 
    locations.push_back (Q); 

    Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, locations);
    iti10->setProperty("name", "bus10");
    iti10->setProperty("lineNumber", "10");
    iti10->setProperty("color", "red");
	
    locations.clear ();
    locations.push_back (R); 
    locations.push_back (F); 
    locations.push_back (S); 

    DrawableBusLine* dbl1 = new DrawableBusLine (iti1);
    DrawableBusLine* dbl2 = new DrawableBusLine (iti2);
    DrawableBusLine* dbl3 = new DrawableBusLine (iti3);
    DrawableBusLine* dbl4 = new DrawableBusLine (iti4);
    DrawableBusLine* dbl5 = new DrawableBusLine (iti5);
    DrawableBusLine* dbl6 = new DrawableBusLine (iti6);
	
	
    {
      DBLComparator cmp (dbl1, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl2, dbl1)); // Following 1, 2 is on the left of 1
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl2)); // Following 1, 1 is on the right of 2
    }
    {
      DBLComparator cmp (dbl2, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl2, dbl1)); // Following 2, 2 is on the left of 1
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl2)); // Following 2, 1 is on the right of 2
    }
    {
      DBLComparator cmp (dbl3, H->getVertex (), H->getVertex ());
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl4)); // Following 3, 3 is on the right of 4
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl4, dbl3)); // Following 3, 4 is on the left of 3
    }
    {
      DBLComparator cmp (dbl4, H->getVertex (), H->getVertex ());
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl4)); // Following 4, 3 is on the right of 4
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl4, dbl3)); // Following 4, 4 is on the left of 3
    }
    {
      DBLComparator cmp (dbl1, H->getVertex (), H->getVertex ());
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl3, dbl4)); // Following 1, 3 is on the left of 4
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl4, dbl3)); // Following 1, 4 is on the right of 3
    }
    {
      DBLComparator cmp (dbl1, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl1, dbl5)); // Following 1, 1 is on the left of 5
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl5, dbl1)); // Following 1, 5 is on the right of 1
    }
    {
      DBLComparator cmp (dbl1, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl3, dbl5)); // Following 1, 3 is on the right of 5
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl5, dbl3)); // Following 1, 5 is on the left of 1
    }
    {
      DBLComparator cmp (dbl1, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl1, dbl6)); // Following 1, 1 is on the right of 6
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl6, dbl1)); // Following 1, 6 is on the left of 1
    }
    {
      DBLComparator cmp (dbl5, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl3, dbl4)); // Following 5, 3 is on the left of 4
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl4, dbl3)); // Following 5, 4 is on the right of 3
    }
    {
      DBLComparator cmp (dbl6, F->getVertex (), F->getVertex ());
      CPPUNIT_ASSERT_EQUAL (1, cmp (dbl6, dbl5)); // Following 6, 6 is on the right of 5
      CPPUNIT_ASSERT_EQUAL (0, cmp (dbl5, dbl6)); // Following 6, 5 is on the left of 6
    }
	
		
	
    // Finally dump a map
    /* std::ofstream of ("/home/mjambert/map2.ps");
    Map map (&topo, of, Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
    map.dump();*/
	
  }





}
