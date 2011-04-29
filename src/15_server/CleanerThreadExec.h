#ifndef SYNTHESE_SERVER_CLEANERTHREADEXEC_H
#define SYNTHESE_SERVER_CLEANERTHREADEXEC_H

#include "01_util/threads/ThreadExec.h"


#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem/path.hpp>
#include <map>

namespace synthese
{

namespace server
{



/** Cleaner thread execution body.

This thread is aimed at cleaning temporary folders on a regular basis.

@ingroup m15
*/
class CleanerThreadExec : public synthese::util::ThreadExec
{
 private:

    std::map<boost::filesystem::path, boost::posix_time::time_duration> _tempDirectories;
    boost::posix_time::ptime _lastCheck;


 public:

    CleanerThreadExec ();

    //! @name Update methods
    //@{
    void addTempDirectory (const boost::filesystem::path& path,
			   boost::posix_time::time_duration checkPeriod);

    void removeTempDirectory (const boost::filesystem::path& path);
    //@}


    /** Execution body.
     */
    void loop();

 private:


};


}
}



#endif

