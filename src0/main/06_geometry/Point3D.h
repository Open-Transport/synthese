#ifndef SYNTHESE_GEOMETRY_POINT3D_H
#define SYNTHESE_GEOMETRY_POINT3D_H

#include "06_geometry/Point2D.h"


#include <iostream>



namespace synthese
{
namespace geometry
{



/** Point3D class
    @ingroup m06
*/
class Point3D : public Point2D
{
    private:

        double _z; 

    public:

        Point3D (double x, double y, double z);
        ~Point3D () {}


        //! @name Getters/Setters
        //@{
        double getZ () const { return _z; }
	void setZ (double z) { _z = z; }
        //@}

};


bool operator== ( const Point3D& op1, const Point3D& op2 );
bool operator!= ( const Point3D& op1, const Point3D& op2 );

std::ostream& operator<< ( std::ostream& os, const Point3D& op );


}

}

#endif





