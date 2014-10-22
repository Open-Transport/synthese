
/** Thread class implementation.
	@file Thread.cpp

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

#include "Thread.h"
#include "ThreadException.h"
#include "Log.h"

#include <boost/thread/xtime.hpp>
#include <boost/lexical_cast.hpp>
#ifdef __gnu_linux__
#include <sys/prctl.h>
#endif
using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		 const std::string Thread::DEFAULT_NAME_PREFIX = "thread_";
		 int Thread::_NbThreads = 0;



		Thread::Thread(
			ThreadExec* exec,
			const std::string& name,
			int loopDelay
		): _name ((name == "") ? DEFAULT_NAME_PREFIX + lexical_cast<string>(_NbThreads++) : name),
			_exec (exec),
			_loopDelay (loopDelay),
			_state (new ThreadState (NOT_STARTED)),
			_nbLoops (new unsigned long (0)),
			_stateMutex (new boost::mutex ()),
			_nbLoopsMutex (new boost::mutex ()),
			_execMutex (new boost::mutex ()
		){
			// Implementation note : _state and _stateMutex are shared pointers because
			// when the thread will be spawned, this object will be first copied
			// and then the operator() will be called on the copy.
			// We want to keep in sync the state of the Thread object used 'internally'
			// by the boost thread, and the one from the originally user created Thread object
			// (which is the one likeky to be asked for its state).

			// Moreover, this way, it is feasible to share the same ThreadExec between different
			// Thread objects. Of course, using proper thread-local variables and mutexes in ThreadExec
			// derived classes is left to user responsibility.

		}



		Thread::~Thread ()
		{
		}



		const std::string&
		Thread::getName () const
		{
			return _name;
		}






		void
		Thread::start ()
		{
			if (getState () != NOT_STARTED) throw ThreadException ("Thread was already started.");
			boost::thread* thread = new boost::thread (*this);

			// The thread is started immediately, we detach it (by deleting it).
			delete thread;
		}




		void
		Thread::stop ()
		{
			if (getState () == STOPPED) return;
			setState (STOPPED);
		}



		void
		Thread::pause ()
		{
			if (getState () == STOPPED) throw ThreadException ("Thread was stopped.");
			setState (PAUSED);
		}


		void
		Thread::resume ()
		{
			if (getState () != PAUSED) throw ThreadException ("Thread is not paused.");
			setState (READY);
		}





		void
		Thread::operator()()
		{
			try
			{
			execInitialize ();

			ThreadState state = getState ();

			while (state != STOPPED)
			{
				execLoop ();
				Sleep (_loopDelay);
				state = getState ();
			}
			Log::GetInstance ().info ("Thread " + _name +  " is stopped.");
			}
			catch (std::exception& ex)
			{
			Log::GetInstance ().error ("Thread " + _name +  " has crashed.", ex);
			}

			// Finalization is done in ANY case even after a crash
			Log::GetInstance ().info ("Finalizing thread " + _name + "...");

			_exec->finalize ();

			Log::GetInstance ().info ("Finalization done. " + _name + " is dead.");

		}





		/*
		void
		Thread::Yield ()
		{
			boost::thread::yield ();
		}
		*/


		void
		Thread::Sleep (long ms)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
		}






		Thread::ThreadState
		Thread::getState () const
		{
			boost::mutex::scoped_lock lock (*_stateMutex);
			return *_state;
		}



		void
		Thread::setState (ThreadState state)
		{
			boost::mutex::scoped_lock lock (*_stateMutex);
			*_state = state;
		}



		unsigned long
		Thread::getNbLoops () const
		{
			boost::mutex::scoped_lock lock (*_nbLoopsMutex);
			return *_nbLoops;
		}




		void
		Thread::execInitialize ()
		{
#ifdef __gnu_linux__
			// Name the thread at system level to ease debugging
			prctl(PR_SET_NAME, _name.c_str(), 0, 0, 0);
#endif
			setState (INIT);
			Log::GetInstance ().info ("Initializing thread " + _name +  "...");

			_exec->initialize ();
			setState (READY);

			Log::GetInstance ().info ("Thread " + _name +  " is ready.");
		}




		void
		Thread::execLoop ()
		{
			boost::mutex::scoped_lock lock (*_nbLoopsMutex);
			if (getState () != PAUSED) _exec->loop ();
			*_nbLoops = *_nbLoops + 1;
		}





		void
		Thread::waitForReadyState () const
		{
			waitForState (Thread::READY);
		}



		void
		Thread::waitForState (const Thread::ThreadState& state) const
		{
			while (1)
			{
			if (getState () == state)
			{
				break;
			}
			Thread::Sleep (5);
			}
		}



		typedef struct runOnce
		{
			boost::shared_ptr<ThreadExec> _exec;

			runOnce (ThreadExec* exec) : _exec (exec) {}

			void operator()()
			{
				try
				{
				_exec->initialize ();
				_exec->loop ();
				_exec->finalize ();
				}
				catch (std::exception& ex)
				{
				Log::GetInstance ().error ("Thread has crashed.", ex);
				}
			}
		} RunOnceStruct ;




		void
		Thread::RunOnce (ThreadExec* exec)
		{
			RunOnceStruct runOnce (exec);
			runOnce ();


			/* boost::thread* thread = new boost::thread (runOnce);
			// The thread is started immediately, we detach it (by deleting it).
			delete thread; */

		}
}	}
