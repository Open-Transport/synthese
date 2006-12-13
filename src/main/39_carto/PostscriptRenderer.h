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
 public:

    PostscriptRenderer ();
    virtual ~PostscriptRenderer ();

	std::string render(const boost::filesystem::path& tempDir, 
		const std::string& filenamePrefix,
		const synthese::env::Environment* environment,
		synthese::carto::Map& map,
		const synthese::carto::RenderingConfig& config);

    
 private:

    void renderBackground (PostscriptCanvas& _canvas, Map& map);
    void renderLines (PostscriptCanvas& _canvas,Map& map);
    void renderPhysicalStops (PostscriptCanvas& _canvas,Map& map);

    void doDrawCurvedLine (PostscriptCanvas& _canvas,const DrawableLine* dbl);

    void doDrawTriangleArrow (PostscriptCanvas& _canvas,const synthese::env::Point& point, 
                              double angle);
    
    void doDrawSquareStop (PostscriptCanvas& _canvas,const synthese::env::Point& point, 
                           double angle);
    
    void doDrawSquareTerminus (PostscriptCanvas& _canvas,const synthese::env::Point& point, 
			       double angle);

};

}
}

#endif

