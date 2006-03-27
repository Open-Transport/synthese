#ifndef SYNTHESE_CARTO_MAPBACKGROUNDTILE_H
#define SYNTHESE_CARTO_MAPBACKGROUNDTILE_H

#include <boost/filesystem/path.hpp>
#include "Drawable.h"


namespace synthese
{
namespace carto
{


class Map;

class MapBackgroundTile : public Drawable
{
private:

    boost::filesystem::path _path;
    int _pixelWidth;
    int _pixelHeight;
    double _topLeftX;
    double _topLeftY;
    double _bottomRightX;
    double _bottomRightY;

public:


    MapBackgroundTile(const boost::filesystem::path path,
                      int pixelWidth, int pixelHeight,
                      double topLeftX, double topLeftY,
                      double bottomRightX, double bottomRightY);
                      
    virtual ~MapBackgroundTile();
    
    
    const boost::filesystem::path& getPath () const { return _path; }
    double getTopLeftX () const { return _topLeftX; }
    double getTopLeftY () const { return _topLeftY; }
    
    double getWidth () const;
    double getHeight () const;
    
    int getPixelWidth () const { return _pixelWidth; }
    int getPixelHeight () const { return _pixelHeight; }
    
    virtual void preDraw (Map& map, PostscriptCanvas& canvas) const;
    
    virtual void draw (Map& map, PostscriptCanvas& canvas) const;

    virtual void postDraw (Map& map, PostscriptCanvas& canvas) const;
    
    bool operator< (const MapBackgroundTile& rhs) const;
    
    
};

}
}

#endif
