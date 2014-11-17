
/** MapModule class implementation.
	@file MapModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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


#include "01_util/Log.h"


#include "39_map/MapModule.h"
#include "39_map/MapBackgroundManager.h"

#include <boost/filesystem/operations.hpp>


#ifdef UNIX
  #define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
  #define DEFAULT_TEMP_DIR "C:\\temp"
#endif

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace map;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass, MapModule>::FACTORY_KEY("59_map");
	}


	namespace map
	{

	    const std::string MapModule::PARAM_HTTP_TEMP_DIR ("http_temp_dir");
	    const std::string MapModule::PARAM_HTTP_TEMP_URL ("http_temp_url");
	    const std::string MapModule::PARAM_BACKGROUNDS_DIR ("backgrounds_dir");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<MapModule>::NAME("Cartographie");


	    template<> void ModuleClassTemplate<MapModule>::PreInit ()
	    {
			RegisterParameter (MapModule::PARAM_HTTP_TEMP_DIR, DEFAULT_TEMP_DIR, &MapModule::ParameterCallback);
			RegisterParameter (MapModule::PARAM_HTTP_TEMP_URL, "http://localhost/tmp", &MapModule::ParameterCallback);
			RegisterParameter (MapModule::PARAM_BACKGROUNDS_DIR, "backgrounds", &MapModule::ParameterCallback);
	    }

		template<> void ModuleClassTemplate<MapModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<MapModule>::Start()
		{
		}


		template<> void ModuleClassTemplate<MapModule>::End()
		{
			UnregisterParameter(MapModule::PARAM_HTTP_TEMP_DIR);
			UnregisterParameter(MapModule::PARAM_HTTP_TEMP_URL);
			UnregisterParameter(MapModule::PARAM_BACKGROUNDS_DIR);
		}



		template<> void ModuleClassTemplate<MapModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<MapModule>::CloseThread(
			
			){
		}
	}

	namespace map
	{
	    void MapModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if (name == PARAM_HTTP_TEMP_DIR)
			{
				boost::filesystem::path path (value);

				if (boost::filesystem::exists (path) == false)
				{
					Log::GetInstance ().warn (name + " "
							  + path.string () +
							  " does not exist. Using default value instead.");
					SetParameter (name, DEFAULT_TEMP_DIR, false);
				}
			}
			else if (name == PARAM_BACKGROUNDS_DIR)
			{
				boost::filesystem::path path (value);
				MapBackgroundManager::Initialize (path);
			}
		}
	}
}
