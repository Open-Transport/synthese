#include "01_util/threads/Thread.h"
#include "01_util/threads/ThreadExec.h"

#include <iostream>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;




class TestExec : public ThreadExec
{
private:
    int _calls;
    mutable boost::mutex* _callsMutex;

public:

    TestExec ()
	: _calls (-1)
	, _callsMutex (new boost::mutex ())
	{
	}

    ~TestExec ()
	{
	    delete _callsMutex;
	}


    int getCalls ()
	{
	    boost::mutex::scoped_lock lock (*_callsMutex);
	    return _calls;
	}

    void initialize()
	{
	    {
		// Separate block to release lock before sleeping.
		boost::mutex::scoped_lock lock (*_callsMutex);
		_calls = 0;
	    }
	    Thread::Sleep (200);
	}


    void loop ()
	{
	    boost::mutex::scoped_lock lock (*_callsMutex);
	    ++_calls;
	}


    void finalize()
	{
	    boost::mutex::scoped_lock lock (*_callsMutex);
	    _calls = -1;
	}


};






BOOST_AUTO_TEST_CASE (testStartPauseResumeStop)
{

      TestExec* exec = new TestExec ();

      Thread thread (exec);

      BOOST_REQUIRE_EQUAL (Thread::NOT_STARTED, thread.getState ());
      BOOST_REQUIRE_EQUAL (-1, exec->getCalls ());

      thread.start ();

      while (thread.getState () != Thread::INIT) Thread::Sleep (5);

      BOOST_REQUIRE_EQUAL (Thread::INIT, thread.getState ());
      BOOST_REQUIRE_EQUAL (0, exec->getCalls ());

      while (thread.getState () != Thread::READY) Thread::Sleep (5);

      BOOST_REQUIRE_EQUAL (Thread::READY, thread.getState ());
      // The following line can be false : State INIT is set before initialization is done, so
	  // getCalls can return -1 or 0 at this moment
      //BOOST_REQUIRE (exec->getCalls () >= 0);

      thread.pause ();

      BOOST_REQUIRE_EQUAL (Thread::PAUSED, thread.getState ());
      int callsOnPause = exec->getCalls ();
      Thread::Sleep (1000);
      BOOST_REQUIRE_EQUAL (callsOnPause, exec->getCalls ());

      thread.resume ();

      Thread::Sleep (1000);
      BOOST_REQUIRE_EQUAL (Thread::READY, thread.getState ());
      BOOST_REQUIRE (callsOnPause < exec->getCalls ());

      thread.stop ();

      Thread::Sleep (1000);
      BOOST_REQUIRE_EQUAL (Thread::STOPPED, thread.getState ());

}
