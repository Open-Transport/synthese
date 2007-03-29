
#include "01_util/Log.h"


#include "39_map/MapModule.h"
#include "39_map/MapBackgroundManager.h"

#include <boost/filesystem/operations.hpp>
using synthese::util::Log;


#ifdef UNIX
  #define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
  #define DEFAULT_TEMP_DIR "c:/temp"
#endif


namespace synthese
{



	namespace map
	{

	    const std::string MapModule::PARAM_HTTP_TEMP_DIR ("http_temp_dir");
	    const std::string MapModule::PARAM_HTTP_TEMP_URL ("http_temp_url");
	    const std::string MapModule::PARAM_BACKGROUNDS_DIR ("backgrounds_dir");


	    void MapModule::preInit ()
	    {

		RegisterParameter (PARAM_HTTP_TEMP_DIR, DEFAULT_TEMP_DIR, &ParameterCallback);
		RegisterParameter (PARAM_HTTP_TEMP_URL, "http://localhost/tmp", &ParameterCallback);
		RegisterParameter (PARAM_BACKGROUNDS_DIR, "backgrounds", &ParameterCallback);

	    }
	    

	    
	    void 
	    MapModule::ParameterCallback (const std::string& name, 
					  const std::string& value)
	    {
		if (name == PARAM_HTTP_TEMP_DIR) 
		{
		    boost::filesystem::path path (value, boost::filesystem::native);

		    if (boost::filesystem::exists (path) == false)
		    {
			Log::GetInstance ().warn (name + " " 
						  + path.string () + 
						  " does not exist. Using default value instead.");
			SetParameter (name, DEFAULT_TEMP_DIR);
		    } 
		}
		else if (name == PARAM_BACKGROUNDS_DIR)
		{
		    boost::filesystem::path path (value, boost::filesystem::native);
		    MapBackgroundManager::Initialize (path);
		}


	    }


	}
}

