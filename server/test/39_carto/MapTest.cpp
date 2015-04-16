#include "MapTest.h"

#include "01_util/RGBColor.h"
#include "15_env/Point.h"
#include "39_carto/DrawableLine.h"
#include "39_carto/Map.h"


#include <iostream>


using synthese::env::Point;
using synthese::util::RGBColor;




namespace synthese
{
namespace carto
{




  void
  MapTest::setUp ()
  {

  }


  void
  MapTest::tearDown()
  {

  }


  void
  MapTest::testMap1 ()
  {
      // This is a bug fix test.
/*
      const Point A1 (527815, 1845779);
      const Point B1 (527961, 1845458);
      const Point C1 (527957, 1845141);
      const Point D1 (528036, 1844685);
      const Point E1 (528283, 1844689);

      const Point A2 (527815, 1845779);
      const Point B2 (527961, 1845458);
      const Point C2 (527957, 1845141);
      const Point D2 (527952, 1844573);


      std::vector<const Point*> points1;
      points1.push_back (&A1);
      points1.push_back (&B1);
      points1.push_back (&C1);
      points1.push_back (&D1);
      points1.push_back (&E1);

      std::vector<const Point*> points2;
      points2.push_back (&A2);
      points2.push_back (&B2);
      points2.push_back (&C2);
      points2.push_back (&D2);

      DrawableLine* line1 = new DrawableLine ("1", points1, "L1", RGBColor ("blue"));
      DrawableLine* line2 = new DrawableLine ("2", points2, "L2", RGBColor ("red"));

      std::set<DrawableLine*> selectedLines;
      selectedLines.insert (line1);
      selectedLines.insert (line2);

      Rectangle realFrame (524008,
			   1840080,
			   (531908 - 524008),
			   (1847980 - 1840080));

      Map map (selectedLines, realFrame, 800, 800);

      map.prepare ();

      double spacing = 6.0;
      line1->prepare (map, spacing);
      line2->prepare (map, spacing);

      const std::vector<synthese::env::Point>& shiftedPoints1 = line1->getShiftedPoints ();
      const std::vector<synthese::env::Point>& shiftedPoints2 = line2->getShiftedPoints ();

      for (int i=0; i<shiftedPoints1.size (); ++i)
      {
	  std::cout << "Point " << i << " : " << shiftedPoints1[i].getX () << ", " << shiftedPoints1[i].getY () << std::endl;
      }

      std::cout << "--- Line 2 --- " << shiftedPoints2.size () << std::endl;
      for (int i=0; i<shiftedPoints2.size (); ++i)
      {
	  std::cout << "Point " << i << " : " << shiftedPoints2[i].getX () << ", " << shiftedPoints2[i].getY () << std::endl;
      }

*/
/*
      {
	  DrawableLineComparator cmp (&line1, &A1, &A2);
	  CPPUNIT_ASSERT_EQUAL (1, cmp (&line2, &line1)); // Following 1, 2 is on the right of 1
	  CPPUNIT_ASSERT_EQUAL (0, cmp (&line1, &line2)); // Following 1, 1 is on the left of 2
      }
*/
      // CPPUNIT_ASSERT (grid.isMarked (3, 4) == false);
  }






}
}


