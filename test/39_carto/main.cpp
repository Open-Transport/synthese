#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>

#include "DrawableLineComparatorTest.h"
#include "MapTest.h"
#include "TileGridTest.h"



CPPUNIT_TEST_SUITE_REGISTRATION(synthese::carto::DrawableLineComparatorTest);
CPPUNIT_TEST_SUITE_REGISTRATION(synthese::carto::MapTest);
CPPUNIT_TEST_SUITE_REGISTRATION(synthese::carto::TileGridTest);



int main( int argc, char **argv )
{

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;

  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );

  runner.run( controller );

  std::cerr << std::endl;

  // Print test in a compiler compatible format.
  CppUnit::CompilerOutputter outputter( &result, std::cerr );
  outputter.write();


  // runner.run( controller );

  return result.wasSuccessful() ? 0 : 1;
}

