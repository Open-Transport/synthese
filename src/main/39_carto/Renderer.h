#ifndef SYNTHESE_CARTO_RENDERER_H
#define SYNTHESE_CARTO_RENDERER_H


#include <iostream>
#include <string>
#include "RenderingConfig.h"



namespace synthese
{


namespace carto
{
    class Map;


class Renderer
{
 protected:

    RenderingConfig _config;

 public:

    Renderer (const RenderingConfig& config);
    virtual ~Renderer ();

    virtual void render (Map& map) = 0;
    
    
 private:
    
    Renderer (const Renderer&);
    Renderer& operator= (const Renderer& rhs);

};

}
}

#endif
