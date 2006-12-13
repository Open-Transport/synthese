#include "ThreadTest.h"

#include "01_util/Thread.h"
#include "01_util/ThreadExec.h"

#include <iostream>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace util
{


  void 
  ThreadTest::setUp () 
  {

  }


  void 
  ThreadTest::tearDown() 
  {

  } 




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
    
    



  void
  ThreadTest::testStartPauseResumeStop ()
  {

      TestExec* exec = new TestExec ();

      Thread thread (exec);
      
      CPPUNIT_ASSERT_EQUAL (Thread::NOT_STARTED, thread.getState ());
      CPPUNIT_ASSERT_EQUAL (-1, exec->getCalls ());
      
      thread.start ();

      while (thread.getState () != Thread::INIT) Thread::Sleep (5);

      CPPUNIT_ASSERT_EQUAL (Thread::INIT, thread.getState ());
      CPPUNIT_ASSERT_EQUAL (0, exec->getCalls ());

      while (thread.getState () != Thread::READY) Thread::Sleep (5);

      CPPUNIT_ASSERT_EQUAL (Thread::READY, thread.getState ());
      CPPUNIT_ASSERT (exec->getCalls () >= 0);

      thread.pause ();

      CPPUNIT_ASSERT_EQUAL (Thread::PAUSED, thread.getState ());
      int callsOnPause = exec->getCalls ();
      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (callsOnPause, exec->getCalls ());

      thread.resume ();

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (Thread::READY, thread.getState ());
      CPPUNIT_ASSERT (callsOnPause < exec->getCalls ());

      thread.stop ();

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (Thread::STOPPED, thread.getState ());

  }



}
}


