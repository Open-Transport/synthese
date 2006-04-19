#ifndef SYNTHESE_CARTO_HTMLMAPRENDERER_H
#define SYNTHESE_CARTO_HTMLMAPRENDERER_H

#include "Renderer.h"

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


class HtmlMapRenderer : public Renderer
{
 private:

    std::ostream& _output; //!< Stream used for HTML map generation
    std::string _mapImgFilename; //!< Filename of the img used in generated HTML map
    
 public:


    /** Constructor.
     * @param config Rendering configuration.
     * @param mapImgFilename Filename (without path) of the img used in generated HTML map
     * @param output Output stream
     */
    HtmlMapRenderer (const RenderingConfig& config, 
		     const std::string& mapImgFilename,
		     std::ostream& output);

    virtual ~HtmlMapRenderer ();

    void render (Map& map);
    
 private:

    void renderLines (Map& map);
    void renderPhysicalStops (Map& map);

 
};

}
}

#endif
