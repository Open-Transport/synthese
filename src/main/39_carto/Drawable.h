#ifndef SYNTHESE_CARTO_DRAWABLE_H
#define SYNTHESE_CARTO_DRAWABLE_H

#include <string>


namespace synthese
{
namespace carto
{


class Map;
class PostscriptCanvas;



class Drawable
{
	
protected:

    Drawable();
    virtual ~Drawable();
	
public:
	
    virtual void preDraw (Map& map, PostscriptCanvas& canvas) const = 0;
    
    virtual void draw (Map& map, PostscriptCanvas& canvas) const = 0;
	
    virtual void postDraw (Map& map, PostscriptCanvas& canvas) const = 0;
    
};

}
}

#endif
