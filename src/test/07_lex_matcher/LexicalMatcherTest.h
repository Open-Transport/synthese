#ifndef SYNTHESE_UTIL_LEXICALMATCHERTEST_H
#define SYNTHESE_UTIL_LEXICALMATCHERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#include <vector>
#include <string>

namespace synthese
{
namespace lexmatcher
{


class LexicalMatcherTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(LexicalMatcherTest);
  CPPUNIT_TEST(testBasicMatches);
  CPPUNIT_TEST(testWholeWordMatchingCriterium);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testBasicMatches ();
  void testWholeWordMatchingCriterium ();

  std::vector<std::string> getTestBase1 () const;

};




}
}


#endif 
