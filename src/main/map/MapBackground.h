#ifndef MAPBACKGROUND_H_
#define MAPBACKGROUND_H_

#include <string>

#include <boost/filesystem/path.hpp>

#include "MapBackgroundTile.h"



namespace synmap
{

class MapBackground
{
private:


    const boost::filesystem::path _tileDir;
    
    double _scaleX;
    double _scaleY;
    
    double _minX;
    double _minY;
    double _maxX;
    double _maxY;
    
    double _tileWidth;
    double _tileHeight;

    int _numTilesX;
    int _numTilesY;
    
    MapBackgroundTile*** _tileArray;
    
public:
	MapBackground(const std::string& tileDir);
	virtual ~MapBackground();
    
    const MapBackgroundTile* getTileContaining (double x, double y) const;
    std::pair<int,int> getIndexesOfTileContaining (double x, double y) const;
    
    const std::string getPath () const { return _tileDir.string (); }
    const MapBackgroundTile* getTile (int indexx, int indexy) const;

    double getScaleX () const { return _scaleX; }
    double getScaleY () const { return _scaleY; }
    
    
    void dumpTileGrid ();
    
};

}

#endif /*MAPBACKGROUND_H_*/
