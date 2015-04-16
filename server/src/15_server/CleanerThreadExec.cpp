
/** CleanerThreadExec class implementation.
	@file CleanerThreadExec.cpp

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

#include "CleanerThreadExec.h"

#include "01_util/Log.h"
#include "01_util/threads/Thread.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

using synthese::util::Log;

using namespace boost::posix_time;


namespace synthese
{
namespace server
{

CleanerThreadExec::CleanerThreadExec ()
: ThreadExec ()
, _lastCheck (boost::date_time::microsec_clock<ptime>::local_time ())
{

}



void
CleanerThreadExec::addTempDirectory (const boost::filesystem::path& path,
				 boost::posix_time::time_duration checkPeriod)
{
    // TODO add a lock to be able to add remove directories even
    // when the thread is started
    _tempDirectories.insert (std::make_pair (path, checkPeriod));
}



void
CleanerThreadExec::removeTempDirectory (const boost::filesystem::path& path)
{
    // TODO add a lock to be able to add remove directories even
    // when the thread is started
    _tempDirectories.erase (path);
}







void
CleanerThreadExec::loop ()
{
    ptime checkTime = boost::date_time::microsec_clock<ptime>::local_time ();

    for (std::map<boost::filesystem::path, boost::posix_time::time_duration>::iterator it =
	     _tempDirectories.begin ();
	 it != _tempDirectories.end (); ++it)
    {
	time_duration checkPeriod = it->second;
	time_duration duration = checkTime - _lastCheck;
	if (duration > checkPeriod)
	{
	    const boost::filesystem::path& path = it->first;
	    Log::GetInstance ().debug ("Checking directory " + path.string() + " for cleaning... ");

	    boost::filesystem::directory_iterator end_iter;
	    for ( boost::filesystem::directory_iterator dir_itr( path );
		  dir_itr != end_iter;
		  ++dir_itr )
	    {
		ptime lastWriteTime (from_time_t(last_write_time (*dir_itr)));
		time_duration duration = checkTime - lastWriteTime;
		// 8 hours is the default time to keep files. It should be a parameter as well.
		time_duration limit = hours(8);
		if (duration > limit)
		{
		    // Erase the file
		    boost::filesystem::remove (*dir_itr);
		}
	    }

	}
    }
    _lastCheck = checkTime;

}



}
}


