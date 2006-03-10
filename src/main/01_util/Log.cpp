#include "Log.h"

#include <iomanip>


namespace synthese
{
namespace util
{

const std::string Log::LOG_PREFIX_DEBUG ("DEBUG");
const std::string Log::LOG_PREFIX_INFO  ("INFO ");
const std::string Log::LOG_PREFIX_WARN  ("WARN ");
const std::string Log::LOG_PREFIX_ERROR ("ERROR");
const std::string Log::LOG_PREFIX_FATAL ("FATAL");



Log::Log ( std::ostream& outputStream, Log::Level level )
    : _outputStream (outputStream)
    , _level (level)
{
}


Log::~Log ()
{
    
}


Log::Level 
Log::getLevel () const
{
    return _level;
}



void 
Log::setLevel (Log::Level level)
{
    _level = level;
}

    

void 
Log::debug (const std::string& message)
{
    if (_level > Log::LEVEL_DEBUG) return;
    append (Log::LEVEL_DEBUG, message);
}

void 
Log::debug (const std::string& message, const Exception& exception)
{
    if (_level > Log::LEVEL_DEBUG) return;
    append (Log::LEVEL_DEBUG, message, &exception);
}



void 
Log::info (const std::string& message)
{
    if (_level > Log::LEVEL_INFO) return;
    append (Log::LEVEL_INFO, message);
}



void 
Log::info (const std::string& message, const Exception& exception)
{
    if (_level > Log::LEVEL_INFO) return;
    append (Log::LEVEL_INFO, message, &exception);
}



void 
Log::warn (const std::string& message)
{
    if (_level > Log::LEVEL_WARN) return;
    append (Log::LEVEL_WARN, message);
}



void 
Log::warn (const std::string& message, const Exception& exception)
{
    if (_level > Log::LEVEL_WARN) return;
    append (Log::LEVEL_WARN, message, &exception);
}



void 
Log::error (const std::string& message)
{
    if (_level > Log::LEVEL_ERROR) return;
    append (Log::LEVEL_ERROR, message);
}


void 
Log::error (const std::string& message, const Exception& exception)
{
    if (_level > Log::LEVEL_ERROR) return;
    append (Log::LEVEL_ERROR, message, &exception);
}




void 
Log::fatal (const std::string& message)
{
    if (_level > Log::LEVEL_FATAL) return;
    append (Log::LEVEL_FATAL, message);
}
    


void 
Log::fatal (const std::string& message, const Exception& exception)
{
    if (_level > Log::LEVEL_FATAL) return;
    append (Log::LEVEL_FATAL, message, &exception);
}
    



void 
Log::append (Log::Level level, 
	     const std::string& message, 
	     const Exception* exception)
{
    // Standard io streams are not thread safe.
    // Acquire lock here.
    
    boost::mutex::scoped_lock lock (_ioMutex);

    switch (level) 
    {
    case Log::LEVEL_DEBUG:
	_outputStream << LOG_PREFIX_DEBUG;
	break;
    case Log::LEVEL_INFO:
	_outputStream << LOG_PREFIX_INFO;
	break;
    case Log::LEVEL_WARN:
	_outputStream << LOG_PREFIX_WARN;
	break;
    case Log::LEVEL_ERROR:
	_outputStream << LOG_PREFIX_ERROR;
	break;
    case Log::LEVEL_FATAL:
	_outputStream << LOG_PREFIX_DEBUG;
	break;
    }

    // Append date time
    time ( &_rawLogTime );
    _logTimeInfo = localtime ( &_rawLogTime );

    _outputStream << " # " << std::setfill ('0')
		  << std::setw (4) << (1900 + _logTimeInfo->tm_year) << "/" 
		  << std::setw (2) << (1 + _logTimeInfo->tm_mon) << "/"
		  << std::setw (2) << _logTimeInfo->tm_mday << " "
		  << std::setw (2) << _logTimeInfo->tm_hour << ":"
		  << std::setw (2) << _logTimeInfo->tm_min << ":"
		  << std::setw (2) << _logTimeInfo->tm_sec
		  << " # " << message;

    if (exception != 0)
    {
	_outputStream << " : " << exception;
    }



    _outputStream << std::endl;
    // Locks is automatically released when goes out of scope.
    
}






}
}

