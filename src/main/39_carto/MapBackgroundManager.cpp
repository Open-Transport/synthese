#include "MapBackgroundManager.h"

#include "01_util/Exception.h"

#include "MapBackground.h"


#include <cmath>
#include <limits>
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "01_util/Log.h"

using synthese::util::Log;



namespace fs = boost::filesystem;

using namespace std;


namespace synthese
{
namespace carto
{


boost::filesystem::path MapBackgroundManager::_backgroundsDir;
std::map<std::string, MapBackgroundManager*> MapBackgroundManager::_managers;


MapBackgroundManager::MapBackgroundManager(const boost::filesystem::path& backgroundDir)
{

    Log::GetInstance ().debug ("Initializing backgrounds for " + backgroundDir.string ());

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
    return currentBackground;
}



const MapBackgroundManager*
MapBackgroundManager::GetMapBackgroundManager (const std::string& id)
{
    if (_managers.find (id) == _managers.end ())
    {
	// Create new manager
	boost::filesystem::path backgroundDir (_backgroundsDir / id);
	if (boost::filesystem::exists (backgroundDir) == false)
	{
	    throw synthese::util::Exception ("Undefined background for id " + id);
	}
	MapBackgroundManager* manager = new MapBackgroundManager (backgroundDir);
	_managers.insert (std::make_pair (id, manager));
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
