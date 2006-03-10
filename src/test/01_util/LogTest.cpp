#include "LogTest.h"

#include "01_util/Log.h"

#include <iostream>
#include <sstream>

#include <boost/thread/thread.hpp>


namespace synthese
{
namespace util
{


  void 
  LogTest::setUp () 
  {

  }


  void 
  LogTest::tearDown() 
  {

  } 


  void
  LogTest::testBasicLogging ()
  {
      std::stringstream buffer;
      Log log (buffer);

      log.debug ("message 1");
      log.info ("message 2");
      log.warn ("message 3");
      log.error ("message 4");
      log.fatal ("message 5");

      // Do some assertions later on...

  }






    struct LogThread
    {
	Log& _log;
	const std::string _testMessage;
	
	LogThread (Log& log, const std::string& testMessage) 
	    : _log (log)
	    , _testMessage (testMessage) 
	    {
	    }
	
	void operator()()
	    {
		// Print 1000 times the test message.
		for (int i=0; i<1000; ++i) {
		    _log.debug (_testMessage);
		    // Give some time for other threads to execute.
		    boost::thread::yield ();
		}
	    }
    };
    
    
    
  void
  LogTest::testConcurrentLogging ()
  {
      std::stringstream buffer;
      Log log (buffer);

      boost::thread_group threads;

      std::string testMessage = "This is a test message !";

      LogThread testLog (log, testMessage);

      // Creates one hundred threads printing the test message.
      for (int i=0; i<10; ++i) {
	  threads.create_thread(testLog);
      }
      
      threads.join_all();

      // If the class is thread safe nothing is mixed in the
      // final output. We can just test that all the lines in
      // the stringstream buffer have the same length
      char lineBuffer[256];
      int expectedLineSize = -1;

      while (buffer.eof () == false)
      {
	  buffer.getline (lineBuffer, 256);
	  std::string line (lineBuffer);
	  
	  if (line.size () == 0) break;
	  if (expectedLineSize == -1) expectedLineSize = line.size ();

	  CPPUNIT_ASSERT_EQUAL (expectedLineSize, (int) line.size ());
      } 

  }


}
}

