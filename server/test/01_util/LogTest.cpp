#include "01_util/Log.h"

#include <iostream>
#include <sstream>

#include <boost/thread/thread.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testBasicLogging)
{

      std::stringstream buffer;
      Log& log = Log::GetInstance ();
      log.setOutputStream (&buffer);
      log.setLevel (Log::LEVEL_DEBUG);

      BOOST_REQUIRE_EQUAL (Log::LEVEL_DEBUG, log.getLevel ());

      log.debug ("message 1");
      log.info ("message 2");
      log.warn ("message 3");
      log.error ("message 4");
      log.fatal ("message 5");

      // Default log level is INFO
      char lineBuffer[256];

      {
	  buffer.getline (lineBuffer, 256);
	  std::string line (lineBuffer);
	  BOOST_REQUIRE (line.find ("message 1") != std::string::npos);

	  buffer.getline (lineBuffer, 256);
	  line = lineBuffer;
	  BOOST_REQUIRE (line.find ("message 2") != std::string::npos);

	  buffer.getline (lineBuffer, 256);
	  line = lineBuffer;
	  BOOST_REQUIRE (line.find ("message 3") != std::string::npos);

	  buffer.getline (lineBuffer, 256);
	  line = lineBuffer;
	  BOOST_REQUIRE (line.find ("message 4") != std::string::npos);

	  buffer.getline (lineBuffer, 256);
	  line = lineBuffer;
	  BOOST_REQUIRE (line.find ("message 5") != std::string::npos);

      }

      log.setLevel (Log::LEVEL_ERROR);
      BOOST_REQUIRE_EQUAL (Log::LEVEL_ERROR, log.getLevel ());

      log.debug ("message 6");
      log.info ("message 7");
      log.warn ("message 8");
      log.error ("message 9");
      log.fatal ("message 10");

      {
	  buffer.getline (lineBuffer, 256);
	  std::string line (lineBuffer);
	  BOOST_REQUIRE (line.find ("message 9") != std::string::npos);

	  buffer.getline (lineBuffer, 256);
	  line = lineBuffer;
	  BOOST_REQUIRE (line.find ("message 10") != std::string::npos);

      }

      log.setLevel (Log::LEVEL_NONE);
      BOOST_REQUIRE_EQUAL (Log::LEVEL_NONE, log.getLevel ());

      log.debug ("message 11");
      log.info ("message 12");

      {
	  buffer.getline (lineBuffer, 256);
	  std::string line (lineBuffer);
	  BOOST_REQUIRE (line.empty ());
      }


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
		_log.info (_testMessage);
		// Give some time for other threads to execute.
		boost::thread::yield ();
	    }
	}
};







BOOST_AUTO_TEST_CASE (testConcurrentLogging)
{

      std::stringstream buffer;
      Log& log = Log::GetInstance ();
      log.setOutputStream (&buffer);

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

	  BOOST_REQUIRE_EQUAL (expectedLineSize, (int) line.size ());
      }

}




