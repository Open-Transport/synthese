#ifndef SYNTHESE_SERVER_CLEANERTHREAD_H
#define SYNTHESE_SERVER_CLEANERTHREAD_H

#include "module.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <map>

namespace synthese
{

namespace server
{



/** Cleaner thread execution body.

This thread is aimed at cleaning temporary folders on a regular basis.

@ingroup m70
*/
class CleanerThread
{
 private:

    std::map<boost::filesystem::path, boost::posix_time::time_duration> _tempDirectories;
    boost::posix_time::ptime _lastCheck;


 public:

    CleanerThread ();


    //! @name Update methods
    //@{
    void addTempDirectory (const boost::filesystem::path& path, 
			   boost::posix_time::time_duration checkPeriod);

    void removeTempDirectory (const boost::filesystem::path& path);
    //@}


    /** Execution body
     */
    void operator()();

 private:

    void sleep (int nSeconds);


};


}
}



#endif
