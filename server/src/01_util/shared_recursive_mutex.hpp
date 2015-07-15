/** shared_recursive_mutex class header.
	@file shared_recursive_mutex.hpp
	@author Anthony Williams

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

#ifndef SYNTHESE_util_shared_recursive_mutex_hpp__
#define SYNTHESE_util_shared_recursive_mutex_hpp__

// This class is a copy of boost/thread/pthread/shared_mutex.hpp from Boost 1.42
// with these modifications:
// - readers don't wait for writers. This let readers use the lock recursively
//   without deadlocks. The downside is that writers might have to wait a bit.
// - keep only lock_shared/unlock_shared lock/unlock

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <boost/thread/mutex.hpp>
#pragma GCC diagnostic pop
#include <boost/thread/condition_variable.hpp>
#if BOOST_VERSION > 103900
#include <boost/thread/detail/thread_interruption.hpp>
#else
#include <boost/thread/thread.hpp>
#endif

namespace synthese
{
namespace util
{
	class shared_recursive_mutex
	{
	private:
		struct state_data
		{
			unsigned shared_count;
			bool exclusive;
			bool upgrade;
			bool exclusive_waiting_blocked;
		};


		state_data state;
		boost::mutex state_change;
		boost::condition_variable shared_cond;
		boost::condition_variable exclusive_cond;
		boost::condition_variable upgrade_cond;

		void release_waiters()
		{
			exclusive_cond.notify_one();
			shared_cond.notify_all();
		}


	public:
		shared_recursive_mutex()
		{
			state_data state_={0,0,0,0};
			state=state_;
		}

		~shared_recursive_mutex()
		{
		}

		void lock_shared()
		{
			boost::this_thread::disable_interruption do_not_disturb;
			boost::mutex::scoped_lock lk(state_change);

			// NOTE: modification from boost shared_mutex: don't wait while
			// exclusive_waiting_blocked is true, otherwise a reader might
			// deadlock if it calls lock_shared recursively.
			while(state.exclusive)
			{
				shared_cond.wait(lk);
			}
			++state.shared_count;
		}

		void unlock_shared()
		{
			boost::mutex::scoped_lock lk(state_change);
			bool const last_reader=!--state.shared_count;

			if(last_reader)
			{
				if(state.upgrade)
				{
					state.upgrade=false;
					state.exclusive=true;
					upgrade_cond.notify_one();
				}
				else
				{
					state.exclusive_waiting_blocked=false;
				}
				release_waiters();
			}
		}

		void lock()
		{
			boost::this_thread::disable_interruption do_not_disturb;
			boost::mutex::scoped_lock lk(state_change);

			while(state.shared_count || state.exclusive)
			{
				state.exclusive_waiting_blocked=true;
				exclusive_cond.wait(lk);
			}
			state.exclusive=true;
		}

		void unlock()
		{
			boost::mutex::scoped_lock lk(state_change);
			state.exclusive=false;
			state.exclusive_waiting_blocked=false;
			release_waiters();
		}
	};
}
}
#endif // SYNTHESE_util_shared_recursive_mutex_hpp__
