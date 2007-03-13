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


namespace map
{


class Map;
class PostscriptCanvas;



class DrawablePhysicalStop 
{

public:


private:

    const int _physicalStopId;
    const std::string _name;
    synthese::env::Point _point;

protected:

public:

    DrawablePhysicalStop (const synthese::env::PhysicalStop* physicalStop);
    ~DrawablePhysicalStop ();
	
    //! @name Getters/Setters
    //@{
    int getPhysicalStopId () const;
    const std::string& getName () const;
    const synthese::env::Point& getPoint () const;

    //@}

    //! @name Update methods.
    //@{
    void prepare (Map& map);
    //@}
    
};

}
}

#endif

