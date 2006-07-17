#ifndef SYNTHESE_UTIL_LOG_H
#define SYNTHESE_UTIL_LOG_H



#include <map>
#include <string>
#include <iostream>

#include <time.h>

#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace util
{


/** Thread-safe logging class.
To get an instance of this class, the statis GetLog method must
be invoked. With no argument, the default log is returned.

Be careful : this class is not designed for efficiency,
so log carefully...

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

    static const std::string DEFAULT_LOG_NAME;
    static std::map<std::string, Log*> _logs;
    static Log _defaultLog;

    boost::mutex _ioMutex; //!< For thread safety.

    std::ostream* _outputStream; //!< Log output stream.
    Log::Level _level; //!< Log level.

    time_t _rawLogTime;
    struct tm* _logTimeInfo;


    Log ( std::ostream* outputStream = &std::cout, 
	  Log::Level level = Log::LEVEL_DEBUG );

 public:

    ~Log ();

    static Log& GetInstance (const std::string& logName = DEFAULT_LOG_NAME);

    //! @name Getters/Setters
    //@{
    void setOutputStream (std::ostream* outputStream);

    Log::Level getLevel () const;
    void setLevel (Log::Level level);

    //@}

    //! @name Update methods
    //@{
    void debug (const std::string& message);
	void debug (const std::string& message, const std::exception& exception);

    void info (const std::string& message);
	void info (const std::string& message, const std::exception& exception);

    void warn (const std::string& message);
	void warn (const std::string& message, const std::exception& exception);

    void error (const std::string& message);
    void error (const std::string& message, const std::exception& exception);

    void fatal (const std::string& message);
    void fatal (const std::string& message, const std::exception& exception);

    //@}
    

 private:

    static const std::string LOG_PREFIX_DEBUG;
    static const std::string LOG_PREFIX_INFO;
    static const std::string LOG_PREFIX_WARN;
    static const std::string LOG_PREFIX_ERROR;
    static const std::string LOG_PREFIX_FATAL;

    void append (Log::Level level, 
		 const std::string& message, 
		 const std::exception* exception = 0);

};


}

}
#endif

