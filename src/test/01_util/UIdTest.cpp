#include "UIdTest.h"

#include "01_util/UId.h"



namespace synthese
{
namespace util
{


  void 
  UIdTest::setUp () 
  {

  }


  void 
  UIdTest::tearDown() 
  {

  } 


  void
  UIdTest::testEncodingDecoding ()
  {
      {
	  int tableId = 4;
	  int gridId = 11;
	  int gridNodeId = 7;
	  long objectId = 256788;

	  uid id = encodeUId (tableId, gridId, gridNodeId, objectId);
	  
	  CPPUNIT_ASSERT_EQUAL (tableId, decodeTableId (id));
	  CPPUNIT_ASSERT_EQUAL (gridId, decodeGridId (id));
	  CPPUNIT_ASSERT_EQUAL (gridNodeId, decodeGridNodeId (id));
	  CPPUNIT_ASSERT_EQUAL (objectId, decodeObjectId (id));
	  
	  
      }

  }







}
}

