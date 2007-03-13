#ifndef SYNTHESE_MAP_MAPBACKGROUNDMANAGER_H
#define SYNTHESE_MAP_MAPBACKGROUNDMANAGER_H

#include <string>
#include <vector>
#include <map>

#include <boost/filesystem/path.hpp>




namespace synthese
{
namespace map
{

    
class MapBackground;


class MapBackgroundManager
{
private:

    static boost::filesystem::path _backgroundsDir;
    static std::map<std::string, MapBackgroundManager*> _managers;


    std::vector<const MapBackground*> _backgrounds; //!< Each background is associated with one scale.


    MapBackgroundManager(const boost::filesystem::path& backgroundDir);
    
public:
    
    ~MapBackgroundManager();
    
    

    const MapBackground* getBestScalingBackground (double mapScaleX, 
                                                   double mapScaleY) const;

    static void Initialize ();

    static const MapBackgroundManager* GetMapBackgroundManager (const std::string& id);

    static const boost::filesystem::path& GetBackgroundsDir ();
    static void SetBackgroundsDir (const boost::filesystem::path& backgroundsDir);

private:

    
};

}
}

#endif

