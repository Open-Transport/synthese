#include "01_util/UId.h"

#include <iostream>


#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testEncodingDecoding)
{
    
    int tableId = 4;
    int gridId = 11;
    int gridNodeId = 7;
    long objectId = 256788;
    
    uid id = encodeUId (tableId, gridId, gridNodeId, objectId);
    
    BOOST_REQUIRE_EQUAL (tableId, decodeTableId (id));
    BOOST_REQUIRE_EQUAL (gridId, decodeGridId (id));
    BOOST_REQUIRE_EQUAL (gridNodeId, decodeGridNodeId (id));
    BOOST_REQUIRE_EQUAL (objectId, decodeObjectId (id));
	  
}

