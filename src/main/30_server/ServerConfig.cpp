
#include <boost/filesystem/operations.hpp>

#include "01_util/Exception.h"
#include "01_util/Log.h"

#include "30_server/ServerConfigTableSync.h"
#include "30_server/ServerConfig.h"

using synthese::util::Log;


namespace synthese 
{

namespace server
{


ServerConfig::ServerConfig () 
    : _port (3591)
    , _nbThreads (5)
    , _dataDir (".")
    , _tempDir (".")
    , _httpTempDir (".")
    , _httpTempUrl ("http://")
{
    
}
    


ServerConfig::~ServerConfig () 
{
}



int 
ServerConfig::getPort () const
{
    return _port;
}


void 
ServerConfig::setPort (int port)
{
    _port = port;
}




synthese::util::Log::Level 
ServerConfig::getLogLevel () const
{
    return synthese::util::Log::GetInstance ().getLevel ();
}




void 
ServerConfig::setLogLevel (const synthese::util::Log::Level& level) const
{
    return synthese::util::Log::GetInstance ().setLevel (level);
}




int 
ServerConfig::getNbThreads () const
{
    return _nbThreads;
}


void 
ServerConfig::setNbThreads (int nbThreads)
{
    _nbThreads = nbThreads;
}





const boost::filesystem::path& 
ServerConfig::getDataDir () const
{
    return _dataDir;
}



void 
ServerConfig::setDataDir (const boost::filesystem::path& path)
{
    if (boost::filesystem::exists (path) == false)
    {
	Log::GetInstance ().warn (CONFIG_TABLE_COL_PARAMVALUE_DATADIR + " " +
				  path.string () + " does not exist. Using default value instead.");
    }
    else
    {
	_dataDir = path;
	_dataDir.normalize ();
    }


}



const boost::filesystem::path& 
ServerConfig::getTempDir () const
{
    return _tempDir;
}


void 
ServerConfig::setTempDir (const boost::filesystem::path& path)
{
    if (boost::filesystem::exists (path) == false)
    {
	Log::GetInstance ().warn (CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR + " "
				  + path.string () + " does not exist. Using default value instead.");
    }
    else
    {
	_tempDir = path;
	_tempDir.normalize ();
    }

}



const boost::filesystem::path& 
ServerConfig::getHttpTempDir () const
{
    return _httpTempDir;
}


void 
ServerConfig::setHttpTempDir (const boost::filesystem::path& path)
{
    if (boost::filesystem::exists (path) == false)
    {
	Log::GetInstance ().warn (CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR + " "
				  + path.string () + " does not exist. Using default value instead.");
    }
    else
    {
	_httpTempDir = path;
	_httpTempDir.normalize ();
    }
}




const std::string& 
ServerConfig::getHttpTempUrl () const
{
    return _httpTempUrl;
}

    
void 
ServerConfig::setHttpTempUrl (const std::string& httpTempUrl)
{
    _httpTempUrl = httpTempUrl;
}











}
}
