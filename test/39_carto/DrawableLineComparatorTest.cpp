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
  DrawableLineComparatorTest::testSameLines ()
  {
/* PROBLEM the comparator use fuzzyfied points but the test does not run fuzzyfication
   the comparator should just use points

      const Point A1 (2.0, 2.0);
      const Point B1 (4.0, 4.0);
      const Point C1 (6.0, 6.0);

      const Point A2 (2.0, 2.0);
      const Point B2 (4.0, 4.0);
      const Point C2 (6.0, 6.0);

      std::vector<const Point*> points1;
      points1.push_back (&A1);
      points1.push_back (&B1);
      points1.push_back (&C1);

      std::vector<const Point*> points2;
      points2.push_back (&A2);
      points2.push_back (&B2);
      points2.push_back (&C2);

      DrawableLine line1 ("1", points1, "L1", RGBColor ("blue"));
      DrawableLine line2 ("2", points2, "L2", RGBColor ("red"));

      {
	  DrawableLineComparator cmp (&line1, A1, A2, false);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
      }
      {
	  DrawableLineComparator cmp (&line1, B1, B2);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
      }
      {
	  DrawableLineComparator cmp (&line1, C1, C2);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
      }
*/

  }



  void
  DrawableLineComparatorTest::testTiniestLines0 ()
  {
      /*
       *       D x
       *         |
       *       C x
       *        /|
       *       / |
       *    A x  x B
       *
       *
       * L1 : A -> C -> D
       * L2 : B -> C -> D
       */
/* PROBLEM the comparator use fuzzyfied points but the test does not run fuzzyfication
   the comparator should just use points

      const Point A (0.1, 0.1);
      const Point B (3.1, 0.1);
      const Point C (3.1, 3.1);
      const Point D (3.1, 6.1);

      std::vector<const Point*> points;
      points.push_back (&A);
      points.push_back (&C);
      points.push_back (&D);

      DrawableLine line1 ("1", points, "L1", RGBColor ("red"));

      points.clear ();

      points.push_back (&B);
      points.push_back (&C);
      points.push_back (&D);

      DrawableLine line2 ("2", points, "L2", RGBColor ("black"));

      {
	  DrawableLineComparator cmp (&line1, C, C);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
      }
      {
	  DrawableLineComparator cmp (&line1, D, D);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
      }
*/
  }



  void
  DrawableLineComparatorTest::testTiniestLines1 ()
  {
      /*
       *       C x
       *        /|
       *       / |
       *    A x  x B
       *
       *
       * L1 : A -> C
       * L2 : B -> C
       */
/* PROBLEM the comparator use fuzzyfied points but the test does not run fuzzyfication
   the comparator should just use points

      const Point A (0.1, 0.1);
      const Point B (3.1, 0.1);
      const Point C (3.1, 3.1);

      std::vector<const Point*> points;
      points.push_back (&A);
      points.push_back (&C);

      DrawableLine line1 ("1", points, "L1", RGBColor ("red"));

      points.clear ();

      points.push_back (&B);
      points.push_back (&C);

      DrawableLine line2 ("2", points, "L2", RGBColor ("black"));

      {
	  DrawableLineComparator cmp (&line1, C, C);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
      }
*/
  }






  void
  DrawableLineComparatorTest::testVariousComparisons ()
  {
/* PROBLEM the comparator use fuzzyfied points but the test does not run fuzzyfication
   the comparator should just use points

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

      DrawableLine line1 ("1", points, "L1", RGBColor ("red"));

      points.clear ();
      points.push_back (&J);
      points.push_back (&E);
      points.push_back (&F);
      points.push_back (&G);
      points.push_back (&K);

      DrawableLine line2 ("2", points, "L2", RGBColor ("blue"));

      points.clear ();
      points.push_back (&M);
      points.push_back (&H);
      points.push_back (&G);
      points.push_back (&F);
      points.push_back (&L);

      DrawableLine line3 ("3", points, "L3", RGBColor ("cyan"));
      points.clear ();
      points.push_back (&H);
      points.push_back (&G);
      points.push_back (&F);
      points.push_back (&N);

      DrawableLine line4 ("4", points, "L4", RGBColor ("yellow"));

      points.clear ();
      points.push_back (&F);
      points.push_back (&G);
      points.push_back (&H);

      DrawableLine line5 ("5", points, "L5", RGBColor ("green"));

      points.clear ();
      points.push_back (&K);
      points.push_back (&G);
      points.push_back (&F);
      points.push_back (&E);
      points.push_back (&J);

      DrawableLine line6 ("6", points, "L6", RGBColor ("magenta"));
      {
	  DrawableLineComparator cmp (&line1, F, F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line2, &line1)); // Following 1, 2 is on the left of 1
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line2)); // Following 1, 1 is on the right of 2
      }
      {
	  DrawableLineComparator cmp (&line2, F, F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line2, &line1)); // Following 2, 2 is on the left of 1
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line2)); // Following 2, 1 is on the right of 2
      }
      {
	  DrawableLineComparator cmp (&line3, H, H);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line4)); // Following 3, 3 is on the right of 4
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line4, &line3)); // Following 3, 4 is on the left of 3
      }
      {
	  DrawableLineComparator cmp (&line4, H, H);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line4)); // Following 4, 3 is on the right of 4
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line4, &line3)); // Following 4, 4 is on the left of 3
      }
      {
	  DrawableLineComparator cmp (&line1, H, H);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line4)); // Following 1, 3 is on the left of 4
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line4, &line3)); // Following 1, 4 is on the right of 3
      }
      {
	  DrawableLineComparator cmp (&line1, F, F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line5)); // Following 1, 1 is on the left of 5
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line5, &line1)); // Following 1, 5 is on the right of 1
      }
      {
	  DrawableLineComparator cmp (&line1, F, F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line3, &line5)); // Following 1, 3 is on the right of 5
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line5, &line3)); // Following 1, 5 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line1, F, F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line6)); // Following 1, 1 is on the right of 6
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line6, &line1)); // Following 1, 6 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line5, F, F);
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line4)); // Following 5, 3 is on the left of 4
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line4, &line3)); // Following 5, 4 is on the right of 3
      }
      {
	  DrawableLineComparator cmp (&line6, F, F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line6, &line5)); // Following 6, 6 is on the right of 5
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line5, &line6)); // Following 6, 5 is on the left of 6
      }
*/
  }




  void
  DrawableLineComparatorTest::testLinesWithSharedBeginEnd ()
  {
/* PROBLEM the comparator use fuzzyfied points but the test does not run fuzzyfication
   the comparator should just use points

      const Point A (5.0, 5.0);
      const Point B (10.0, 10.0);
      const Point C (15.0, 15.0);
      const Point D (20.0, 30.0);
      const Point E (40.0, 40.0);
      const Point F (50.0, 40.0);

      std::vector<const Point*> points;
      points.push_back (&A);
      points.push_back (&B);
      points.push_back (&F);

      DrawableLine line1 ("1", points, "L1", RGBColor ("cyan"));
      DrawableLine line2 ("2", points, "L2", RGBColor ("yellow"));

      points.clear ();
      points.push_back (&A);
      points.push_back (&B);
      points.push_back (&C);
      points.push_back (&D);
      points.push_back (&E);
      points.push_back (&F);

      DrawableLine line3 ("3", points, "L3", RGBColor ("green"));

      {
	  DrawableLineComparator cmp (&line3, A, A);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line3)); // Following 3, 1 is on the right of 3
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line1)); // Following 3, 3 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line3, B, B);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line3)); // Following 3, 1 is on the right of 3
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line1)); // Following 3, 3 is on the left of 1
      }
      {
	  DrawableLineComparator cmp (&line3, F, F);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line1, &line3)); // Following 3, 1 is on the right of 3
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line3, &line1)); // Following 3, 3 is on the left of 1
      }
*/
  }




}
}


