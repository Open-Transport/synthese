#include "TileGridTest.h"

#include "39_carto/TileGrid.h"

#include "15_env/Point.h"

#include <iostream>


using synthese::env::Point;




namespace synthese
{
namespace carto
{




  void
  TileGridTest::setUp ()
  {

  }


  void
  TileGridTest::tearDown()
  {

  }


  void
  TileGridTest::testPointMarking ()
  {
      TileGrid grid (10.0, 10.0, 1.0, 1.0);
      grid.markTilesForPoint (Point (1.5, 0.5));
      grid.markTilesForPoint (Point (3.5, 3.5));

      CPPUNIT_ASSERT (grid.isMarked (1, 0) == true);
      CPPUNIT_ASSERT (grid.isMarked (0, 0) == false);
      CPPUNIT_ASSERT (grid.isMarked (0, 1) == false);
      CPPUNIT_ASSERT (grid.isMarked (1, 1) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 1) == false);

      CPPUNIT_ASSERT (grid.isMarked (3, 3) == true);
      CPPUNIT_ASSERT (grid.isMarked (2, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (3, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (3, 4) == false);
  }







  void
  TileGridTest::testLineMarking ()
  {
      TileGrid grid (10.0, 10.0, 1.0, 1.0);
      grid.markTilesForLine (Point (1.5, 0.5), Point (3.5, 3.5));

      CPPUNIT_ASSERT (grid.isMarked (1, 0) == true);
      CPPUNIT_ASSERT (grid.isMarked (1, 1) == true);
      CPPUNIT_ASSERT (grid.isMarked (2, 1) == true);
      CPPUNIT_ASSERT (grid.isMarked (2, 2) == true);
      CPPUNIT_ASSERT (grid.isMarked (3, 2) == true);
      CPPUNIT_ASSERT (grid.isMarked (3, 3) == true);

      CPPUNIT_ASSERT (grid.isMarked (0, 0) == false);
      CPPUNIT_ASSERT (grid.isMarked (0, 1) == false);
      CPPUNIT_ASSERT (grid.isMarked (0, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (0, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (0, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (1, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (1, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (1, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 0) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (2, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (3, 0) == false);
      CPPUNIT_ASSERT (grid.isMarked (3, 1) == false);
      CPPUNIT_ASSERT (grid.isMarked (3, 4) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 0) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 1) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 2) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 3) == false);
      CPPUNIT_ASSERT (grid.isMarked (4, 4) == false);


  }





}
}


