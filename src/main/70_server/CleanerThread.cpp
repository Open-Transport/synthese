#include "CleanerThread.h"


#include "01_util/Conversion.h"

#include <boost/filesystem/operations.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

#include "01_util/Log.h"

using synthese::util::Log;
using synthese::util::Conversion;

using namespace boost::posix_time;


namespace synthese
{
namespace server
{

CleanerThread::CleanerThread () 
: _lastCheck (boost::date_time::microsec_clock<ptime>::local_time ()) 
{

}



void 
CleanerThread::addTempDirectory (const boost::filesystem::path& path, 
				 boost::posix_time::time_duration checkPeriod)
{
    _tempDirectories.insert (std::make_pair (path, checkPeriod));
}



void 
CleanerThread::removeTempDirectory (const boost::filesystem::path& path)
{
    _tempDirectories.erase (path);
}






	
void 
CleanerThread::operator()()
{
    while ( 1 )
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

	sleep (5);
    }

    
}


void 
CleanerThread::sleep (int nSeconds)
{
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    xt.sec += nSeconds; 
    boost::thread::sleep(xt);
} 




}
}

