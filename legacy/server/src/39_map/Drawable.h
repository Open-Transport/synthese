#ifndef SYNTHESE_CARTO_DRAWABLE_H
#define SYNTHESE_CARTO_DRAWABLE_H

#include <string>


namespace synthese
{
namespace map
{


class Map;
class PostscriptCanvas;


/** @ingroup m39 */
class Drawable
{

public:

    typedef enum { DRAW_DIRECTION_W, DRAW_DIRECTION_NW,
		   DRAW_DIRECTION_N, DRAW_DIRECTION_NE,
		   DRAW_DIRECTION_E, DRAW_DIRECTION_SE,
		   DRAW_DIRECTION_S, DRAW_DIRECTION_SW
    } DrawDirection;

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

