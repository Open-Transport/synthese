#ifndef MAPBACKGROUNDMANAGER_H_
#define MAPBACKGROUNDMANAGER_H_

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>




namespace synmap
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

#endif /*MAPBACKGROUNDMANAGER_H_*/
