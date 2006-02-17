#include "MapBackgroundManager.h"

#include "MapBackground.h"


#include <cmath>
#include <limits>
#include <iostream>

#include <boost/filesystem/operations.hpp>

namespace fs = boost::filesystem;

using namespace std;

namespace synmap
{




MapBackgroundManager::MapBackgroundManager(const std::string& backgroundDir)
: _backgroundDir (backgroundDir)
{
   fs::directory_iterator end_iter;
   for ( fs::directory_iterator dir_itr( _backgroundDir );
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
        cout << "Mean diff = " << meanDiff << endl;
        if ((meanDiff < currentDiff) && (meanDiff > 0)) {
            currentDiff = meanDiff;
            currentBackground = mbg;
        }
    }
    cout << "Best scaling background = " << currentBackground->getPath() << endl;
    
    return currentBackground;
}



}
