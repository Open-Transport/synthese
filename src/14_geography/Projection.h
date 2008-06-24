#ifndef SYNTHESE_GEOGRAPHY_PROJECTION_H
#define SYNTHESE_GEOGRAPHY_PROJECTION_H

#include "14_geography/GeoPoint.h"

#include <iostream>



namespace synthese
{

namespace geometry
{
    class Point2D;
}


/** Projection service functions
    @ingroup m09
*/

namespace geography
{


    typedef enum { LAMBERT_93, LAMBERT_II, LAMBERT_IIe } LambertOrigin;



    GeoPoint FromLambertIIe (const geometry::Point2D& p);
    GeoPoint FromLambert (const LambertOrigin& orig, const geometry::Point2D& p);




}

}

#endif





