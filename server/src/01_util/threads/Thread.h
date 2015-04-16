
/** Thread class header.
	@file Thread.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef SYNTHESE_UTIL_THREAD_H
#define SYNTHESE_UTIL_THREAD_H

#include <string>
#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "ThreadExec.h"


namespace synthese
{
namespace util
{

 class ThreadExec;
 class ThreadManager;




/** Base class for thread creation and control.

    Implementation note : a copy of the ThreadExec object is kept for latter
    cloning. Mainly because it is too dangerous to deal with shared_ptr destruction
    and threading all together.

@ingroup m01
*/
class Thread
{
 public:


    typedef enum { NOT_STARTED = 0, INIT, READY, PAUSED, STOPPED } ThreadState;


    static const std::string DEFAULT_NAME_PREFIX;
    static int _NbThreads;


 protected:

    std::string _name;
    boost::shared_ptr<ThreadExec> _exec;

    int _loopDelay;  //!< Loop delay in nanoseconds



 private:

    boost::shared_ptr<ThreadState> _state;
    boost::shared_ptr<unsigned long> _nbLoops; //!< Number of loops executed by this thread
    boost::shared_ptr<boost::mutex> _stateMutex;
    boost::shared_ptr<boost::mutex> _nbLoopsMutex;
    boost::shared_ptr<boost::mutex> _execMutex;


 protected:

 public:

    /** Constructs a Thread object given a ThreadExec pointer.
     */
    Thread (ThreadExec* exec, const std::string& name = "", int loopDelay = 500);

    virtual ~Thread ();

    const std::string& getName () const;


    void start ();
    void pause ();
    void resume ();
    void stop ();

    virtual void operator()();

    //Thread& operator=(const Thread& ref);

    static void Sleep (long ms);

    ThreadState getState () const;

    void waitForState (const Thread::ThreadState& state) const;
    void waitForReadyState () const;


    static void RunOnce (ThreadExec* exec);

 protected:

    void setState (ThreadState state);

    unsigned long getNbLoops () const;

    void execInitialize ();
    void execLoop ();

    friend class ThreadManager;

};


 typedef boost::shared_ptr<Thread> ThreadSPtr;


}

}
#endif

