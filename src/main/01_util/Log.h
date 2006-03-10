#ifndef SYNTHESE_UTIL_LOG_H
#define SYNTHESE_UTIL_LOG_H



#include <string>
#include <iostream>
#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace util
{

/** Thread-safe logging class.
@ingroup m01
*/
class Log
{
 private:

    boost::mutex _ioMutex; //!< For thread safety.

    std::ostream& _outputStream; //!< Log output stream.

 public:

    Log ( std::ostream& outputStream );
    ~Log ();


    //! @name Getters/Setters
    //@{

    //@}

    //! @name Update methods
    //@{
    void debug (const std::string& message);
    void info (const std::string& message);
    void warn (const std::string& message);
    void error (const std::string& message);
    void fatal (const std::string& message);
    //@}
    

 private:

    static const std::string LOG_PREFIX_DEBUG;
    static const std::string LOG_PREFIX_INFO;
    static const std::string LOG_PREFIX_WARN;
    static const std::string LOG_PREFIX_ERROR;
    static const std::string LOG_PREFIX_FATAL;

    typedef enum { LOG_LEVEL_DEBUG, 
		   LOG_LEVEL_INFO, 
		   LOG_LEVEL_WARN, 
		   LOG_LEVEL_ERROR, 
		   LOG_LEVEL_FATAL } LogLevel;

    void append (LogLevel level, const std::string& message);

    Log ( const Log& ref );
    Log& operator= ( const Log& rhs );

};


}

}
#endif

