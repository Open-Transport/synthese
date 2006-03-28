#include "DrawableLineComparatorTest.h"

#include "39_carto/DrawableLineComparator.h"
#include "39_carto/DrawableLine.h"

#include "15_env/Point.h"

#include <iostream>

using synthese::env::Point;
using synthese::util::RGBColor;


namespace synthese
{
namespace carto
{




  void 
  DrawableLineComparatorTest::setUp () 
  {

  }


  void 
  DrawableLineComparatorTest::tearDown() 
  {

  } 



  void 
  DrawableLineComparatorTest::testVariousComparisons () 
  {
      const Point A (2.0, 3.0);
      const Point B (6.0, 6.0);
      const Point C (9.0, 5.0);
      const Point D (12.0, 9.0);
      const Point E (12.0, 13.0);
      const Point F (15.0, 15.0);
      const Point G (18.0, 18.0);
      const Point H (20.0, 20.0);
      const Point I (18.0, 23.0);
      const Point J (8.0, 14.0);
      const Point K (16.0, 22.0);
      const Point L (16.0, 7.0);
      const Point M (26.0, 20.0);
      const Point N (19.0, 8.0);
      const Point O (12.0, 1.0);
      const Point P (14.0, 9.0);
      const Point Q (29.0, 14.0);
      const Point R (10.0, 20.0);
      const Point S (20.0, 10.0);

      std::vector<const Point*> points;
      points.push_back (&A); 
      points.push_back (&B); 
      points.push_back (&C); 
      points.push_back (&D); 
      points.push_back (&E); 
      points.push_back (&F); 
      points.push_back (&G); 
      points.push_back (&H); 
      points.push_back (&I); 
      
      DrawableLine line1 (points, "L1", RGBColor ("red"));

      points.clear ();
      points.push_back (&J); 
      points.push_back (&E); 
      points.push_back (&F); 
      points.push_back (&G); 
      points.push_back (&K); 

      DrawableLine line2 (points, "L2", RGBColor ("blue"));

      points.clear ();
      points.push_back (&M); 
      points.push_back (&H); 
      points.push_back (&G); 
      points.push_back (&F); 
      points.push_back (&L); 

      DrawableLine line3 (points, "L3", RGBColor ("cyan"));
      points.clear ();
      points.push_back (&H); 
      points.push_back (&G); 
      points.push_back (&F); 
      points.push_back (&N); 

      DrawableLine line4 (points, "L4", RGBColor ("yellow"));
	
      points.clear ();
      points.push_back (&F); 
      points.push_back (&G); 
      points.push_back (&H); 

      DrawableLine line5 (points, "L5", RGBColor ("green"));
	
      points.clear ();
      points.push_back (&K); 
      points.push_back (&G); 
      points.push_back (&F); 
      points.push_back (&E); 
      points.push_back (&J); 

      DrawableLine line6 (points, "L6", RGBColor ("magenta"));
      {
	  DrawableLineComparator cmp (&line1, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line2, &line1)); // Following 1, 2 is on the left of 1
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line2)); // Following 1, 1 is on the right of 2
      }
      {
	  DrawableLineComparator cmp (&line2, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line2, &line1)); // Following 2, 2 is on the left of 1
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line2)); // Following 2, 1 is on the right of 2
      }
      {
	  DrawableLineComparator cmp (&line3, &H, &H);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line4)); // Following 3, 3 is on the right of 4
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line4, &line3)); // Following 3, 4 is on the left of 3
      }
      {
	  DrawableLineComparator cmp (&line4, &H, &H);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line4)); // Following 4, 3 is on the right of 4
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line4, &line3)); // Following 4, 4 is on the left of 3
      }
      {
	  DrawableLineComparator cmp (&line1, &H, &H);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line4)); // Following 1, 3 is on the left of 4
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line4, &line3)); // Following 1, 4 is on the right of 3
      }
      {
	  DrawableLineComparator cmp (&line1, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line5)); // Following 1, 1 is on the left of 5
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line5, &line1)); // Following 1, 5 is on the right of 1
      }
      {
	  DrawableLineComparator cmp (&line1, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line5)); // Following 1, 3 is on the right of 5
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line5, &line3)); // Following 1, 5 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line1, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line6)); // Following 1, 1 is on the right of 6
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line6, &line1)); // Following 1, 6 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line5, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line4)); // Following 5, 3 is on the left of 4
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line4, &line3)); // Following 5, 4 is on the right of 3
      }
      {
	  DrawableLineComparator cmp (&line6, &F, &F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line6, &line5)); // Following 6, 6 is on the right of 5
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line5, &line6)); // Following 6, 5 is on the left of 6
      }
      

  }



}
}

