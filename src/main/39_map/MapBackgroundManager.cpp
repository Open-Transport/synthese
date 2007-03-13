#include "MapBackgroundManager.h"

#include "01_util/Exception.h"

#include "MapBackground.h"


#include <cmath>
#include <limits>
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "01_util/Log.h"
#include "01_util/Conversion.h"

using synthese::util::Log;
using synthese::util::Conversion;



namespace fs = boost::filesystem;

using namespace std;


namespace synthese
{
namespace map
{


boost::filesystem::path MapBackgroundManager::_backgroundsDir;
std::map<std::string, MapBackgroundManager*> MapBackgroundManager::_managers;


MapBackgroundManager::MapBackgroundManager(const boost::filesystem::path& backgroundDir)
{

    Log::GetInstance ().debug ("Initializing backgrounds in " + backgroundDir.string ());

    // Go through each scale directory
    fs::directory_iterator end_iter;
   for ( fs::directory_iterator dir_itr( backgroundDir );
          dir_itr != end_iter;
          ++dir_itr )
    {
        std::string dirpath = dir_itr->string();

        const MapBackground* background = new MapBackground (dirpath);
        _backgrounds.push_back (background);
    }
}




MapBackgroundManager::~MapBackgroundManager()
{
    for (std::vector<const MapBackground*>::const_iterator iter = _backgrounds.begin (); 
         iter < _backgrounds.end (); 
         ++iter)
    {
        delete *iter;   
    }
    _backgrounds.clear ();
}



const MapBackground* 
MapBackgroundManager::getBestScalingBackground (double mapScaleX, 
                                                double mapScaleY) const
{
    double currentDiff = std::numeric_limits<double>::max ();
    const MapBackground* currentBackground = 0;
    for (std::vector<const MapBackground*>::const_iterator iter = _backgrounds.begin (); 
         iter < _backgrounds.end (); 
         ++iter)
    {
        const MapBackground* mbg = *iter;
        double diffScaleX = mapScaleX - mbg->getScaleX ();
        double diffScaleY = mapScaleY - mbg->getScaleY ();
        double meanDiff = std::abs ((diffScaleX + diffScaleY) / 2.0);
        if ((meanDiff < currentDiff) && (meanDiff > 0)) {
            currentDiff = meanDiff;
            currentBackground = mbg;
        }
    }
	if (currentBackground != 0) 
	{
		Log::GetInstance ().info ("Best scale for " + 
			Conversion::ToString (mapScaleX) + ", " + 
			Conversion::ToString (mapScaleY) + " : " 
			+ Conversion::ToString (currentBackground->getScaleX ()) + ", "
			+ Conversion::ToString (currentBackground->getScaleY ()));
	}


    return currentBackground;
}



void 
MapBackgroundManager::Initialize ()
{
	if (fs::exists (_backgroundsDir) == false)
	{
		Log::GetInstance ().warn ("Map backgrounds dir does not exist : " + _backgroundsDir.string ());
		return;
	}

	// Create all managers
	boost::filesystem::path backgroundDir (_backgroundsDir);
    fs::directory_iterator end_iter;
   for ( fs::directory_iterator dir_itr( _backgroundsDir );
          dir_itr != end_iter;
          ++dir_itr )
    {
        std::string dirpath = dir_itr->leaf();

    	MapBackgroundManager* manager = new MapBackgroundManager (_backgroundsDir / dirpath);
	    _managers.insert (std::make_pair (dirpath, manager));
    }
    
}





const MapBackgroundManager*
MapBackgroundManager::GetMapBackgroundManager (const std::string& id)
{
    if (_managers.find (id) == _managers.end ())
    {
        throw synthese::util::Exception ("Missing background directory " + id);
    }

    return _managers.find (id)->second;
}





const boost::filesystem::path& 
MapBackgroundManager::GetBackgroundsDir ()
{
    return _backgroundsDir;
}



void 
MapBackgroundManager::SetBackgroundsDir (const boost::filesystem::path& backgroundsDir)
{
    _backgroundsDir = backgroundsDir;
}





}
}

