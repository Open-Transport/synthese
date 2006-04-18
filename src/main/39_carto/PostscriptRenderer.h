#ifndef SYNTHESE_CARTO_POSTSCRIPTRENDERER_H
#define SYNTHESE_CARTO_POSTSCRIPTRENDERER_H

#include "Renderer.h"
#include "PostscriptCanvas.h"

#include "01_util/RGBColor.h"
#include "15_env/Point.h"

#include <iostream>
#include <string>
#include <vector>



namespace synthese
{


namespace carto
{

    class DrawableLine;


class PostscriptRenderer : public Renderer
{
 private:
    
    PostscriptCanvas _canvas;
	
 public:

    PostscriptRenderer (const RenderingConfig& config, std::ostream& output);
    virtual ~PostscriptRenderer ();

    void render (Map& map);
    
 private:

    void renderBackground (Map& map);
    void renderLines (Map& map);
    void renderPhysicalStops (Map& map);

    void doDrawCurvedLine (const DrawableLine* dbl);

    void doDrawTriangleArrow (const synthese::env::Point& point, 
                              double angle);
    
    void doDrawSquareStop (const synthese::env::Point& point, 
                           double angle);
    
    void doDrawSquareTerminus (const synthese::env::Point& point, 
			       double angle);

};

}
}

#endif
