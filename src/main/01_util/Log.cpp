#include "Log.h"



namespace synthese
{
namespace util
{

const std::string Log::LOG_PREFIX_DEBUG ("DEBUG");
const std::string Log::LOG_PREFIX_INFO  ("INFO ");
const std::string Log::LOG_PREFIX_WARN  ("WARN ");
const std::string Log::LOG_PREFIX_ERROR ("ERROR");
const std::string Log::LOG_PREFIX_FATAL ("FATAL");



Log::Log ( std::ostream& outputStream )
: _outputStream (outputStream)
{
}


Log::~Log ()
{
    
}
    

void 
Log::debug (const std::string& message)
{
    append (LOG_LEVEL_DEBUG, message);
}



void 
Log::info (const std::string& message)
{
    append (LOG_LEVEL_INFO, message);
}


void 
Log::warn (const std::string& message)
{
    append (LOG_LEVEL_WARN, message);
}



void 
Log::error (const std::string& message)
{
    append (LOG_LEVEL_ERROR, message);
}



void 
Log::fatal (const std::string& message)
{
    append (LOG_LEVEL_FATAL, message);
}
    



void 
Log::append (LogLevel level, const std::string& message)
{
    // Standard io streams are not thread safe.
    // Acquire lock here.
    
    boost::mutex::scoped_lock lock (_ioMutex);

    switch (level) 
    {
    case LOG_LEVEL_DEBUG:
	_outputStream << LOG_PREFIX_DEBUG << " " << message << std::endl;
	break;
    case LOG_LEVEL_INFO:
	_outputStream << LOG_PREFIX_INFO << " " << message << std::endl;
	break;
    case LOG_LEVEL_WARN:
	_outputStream << LOG_PREFIX_WARN << " " << message << std::endl;
	break;
    case LOG_LEVEL_ERROR:
	_outputStream << LOG_PREFIX_ERROR << " " << message << std::endl;
	break;
    case LOG_LEVEL_FATAL:
	_outputStream << LOG_PREFIX_DEBUG << " " << message << std::endl;
	break;
    }

    // Locks is automatically released when goes out of scope.
    
}






}
}

