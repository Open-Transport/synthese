#ifndef SYNTHESE_UTIL_LOG_H
#define SYNTHESE_UTIL_LOG_H



#include <string>
#include <iostream>

#include <time.h>

#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace util
{

    class Exception;

/** Thread-safe logging class.
@ingroup m01
*/
class Log
{
 public:

    typedef enum { LEVEL_DEBUG = 0, 
		   LEVEL_INFO, 
		   LEVEL_WARN, 
		   LEVEL_ERROR, 
		   LEVEL_FATAL,
		   LEVEL_NONE } Level;

 private:

    boost::mutex _ioMutex; //!< For thread safety.

    std::ostream& _outputStream; //!< Log output stream.
    Log::Level _level; //!< Log level.

    time_t _rawLogTime;
    struct tm* _logTimeInfo;

 public:


    Log ( std::ostream& outputStream, Log::Level level = Log::LEVEL_INFO );
    ~Log ();


    //! @name Getters/Setters
    //@{
    Log::Level getLevel () const;
    void setLevel (Log::Level level);

    //@}

    //! @name Update methods
    //@{
    void debug (const std::string& message);
    void debug (const std::string& message, const Exception& exception);

    void info (const std::string& message);
    void info (const std::string& message, const Exception& exception);

    void warn (const std::string& message);
    void warn (const std::string& message, const Exception& exception);

    void error (const std::string& message);
    void error (const std::string& message, const Exception& exception);

    void fatal (const std::string& message);
    void fatal (const std::string& message, const Exception& exception);

    //@}
    

 private:

    static const std::string LOG_PREFIX_DEBUG;
    static const std::string LOG_PREFIX_INFO;
    static const std::string LOG_PREFIX_WARN;
    static const std::string LOG_PREFIX_ERROR;
    static const std::string LOG_PREFIX_FATAL;

    void append (Log::Level level, 
		 const std::string& message, 
		 const Exception* exception = 0);

    Log ( const Log& ref );
    Log& operator= ( const Log& rhs );

};


}

}
#endif

