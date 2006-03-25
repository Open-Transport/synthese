#ifndef SYNTHESE_CARTO_MAPBACKGROUNDMANAGER_H
#define SYNTHESE_CARTO_MAPBACKGROUNDMANAGER_H

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>




namespace synthese
{
namespace carto
{

    
class MapBackground;


class MapBackgroundManager
{
private:
    const boost::filesystem::path _backgroundDir;
    std::vector<const MapBackground*> _backgrounds;
    
public:
    
    MapBackgroundManager(const std::string& backgroundDir);
    virtual ~MapBackgroundManager();
    
    const MapBackground* getBestScalingBackground (double mapScaleX, 
                                                   double mapScaleY) const;
private:

    
};

}
}

#endif
