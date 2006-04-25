#ifndef SYNTHESE_CARTO_MAPINFORENDERER_H
#define SYNTHESE_CARTO_MAPINFORENDERER_H

#include "Renderer.h"

#include "01_util/RGBColor.h"
#include "15_env/Point.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>



namespace synthese
{


namespace carto
{

    class DrawableLine;


class MapInfoRenderer : public Renderer
{
 private:

    static const std::string ZIP_BIN;

    boost::filesystem::path _zipOutput; //!< File used for MapInfo zip (containing MID/MIF) generation
    
 public:


    /** Constructor.
     * @param config Rendering configuration.
     * @param output Output stream
     */
    MapInfoRenderer (const RenderingConfig& config, 
		     const boost::filesystem::path& zipOutput);

    virtual ~MapInfoRenderer ();

    void render (Map& map);
    
 
};

}
}

#endif
