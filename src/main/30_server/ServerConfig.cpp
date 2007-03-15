
/** ServerConfig class implementation.
	@file ServerConfig.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <boost/filesystem/operations.hpp>

#include "01_util/Exception.h"
#include "01_util/Log.h"

#include "30_server/ServerConfigTableSync.h"
#include "30_server/ServerConfig.h"

#ifdef UNIX
	#define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
#define DEFAULT_TEMP_DIR getenv("TEMP") ? getenv("TEMP") : ".", boost::filesystem::native
#endif


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
    , _httpTempDir (DEFAULT_TEMP_DIR)
    , _httpTempUrl ("http://localhost/tmp")
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
    synthese::util::Log::GetInstance ().setLevel (level);
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

