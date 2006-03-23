#ifndef SYNTHESE_CARTO_GEOMETRY_H
#define SYNTHESE_CARTO_GEOMETRY_H


#include <vector>
#include "XYPoint.h"


namespace synthese
{
namespace carto
{



	
    /** Calculates the oriented angle formed by vectors -ba> and -bc>.
     * @return The angle in radians on [-PI..PI] domain.
     */
    double calculateAngle (const XYPoint& a, const XYPoint& b, const XYPoint& c);


    /** Calculates the euclidian distance between points a and b.
     * @return The calculated distance.
     */
    double calculateDistance (const XYPoint& a, const XYPoint& b);
	

    /** Returns symetric of a regarding b
     */
    XYPoint calculateSymetric (const XYPoint& a, const XYPoint& b);
    
	
    std::pair<double, double>
	calculateAffineEquation (const XYPoint& p0, const XYPoint& p1);
	

    /** Converts the given angle to degrees.
     * @param angle The angle in radians.
     * @return The angle in radians.
     */
    double toDegrees (double angle);
	

    /** Calculates the intersection between two affines defined by 
     * y = a1.x + b1
     * y = a2.x + b2
     *
     * @param a1 
     * @param b1 
     * @param a2 
     * @param b2 
     * @return The intersection point.
     */
    XYPoint calculateIntersection (double a1, double b1, double a2, double b2);


    /** Calculates the intersection between two affines.
     * The first affine is defined by two points  : p0 and p1.
     * The second affine is defined by two points : p2 and p3.
     *
     @param p0 
     @param p1 
     @param p2 
     @param p3 
     @return The intersection point.
    */
    XYPoint calculateIntersection (const XYPoint& p0, const XYPoint& p1, 
				   const XYPoint& p2, const XYPoint& p3);
    


}
}

#endif


