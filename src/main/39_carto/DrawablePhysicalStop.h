#ifndef SYNTHESE_CARTO_DRAWABLEPHYSICALSTOP_H
#define SYNTHESE_CARTO_DRAWABLEPHYSICALSTOP_H

#include <string>

#include "Drawable.h"
#include "15_env/Point.h"


namespace synthese
{

namespace env
{
    class PhysicalStop;
}


namespace carto
{


class Map;
class PostscriptCanvas;



class DrawablePhysicalStop : public Drawable
{

public:


private:

    const std::string _name;
    const synthese::env::Point _point;

protected:

public:

    DrawablePhysicalStop (const synthese::env::PhysicalStop* physicalStop);
    ~DrawablePhysicalStop ();
	

    void preDraw (Map& map, PostscriptCanvas& canvas) const;
    
    void draw (Map& map, PostscriptCanvas& canvas) const;
	
    void postDraw (Map& map, PostscriptCanvas& canvas) const;
    
};

}
}

#endif
