#include "Point3D.h"




namespace synthese
{
namespace geometry
{



Point3D::Point3D (double x, double y, double z)
    : Point2D (x, y)
    , _z (z)
{
}



    

bool
operator== ( const Point3D& op1, const Point3D& op2 )
{
    return 
	(op1.getX () == op2.getX ()) && 
	(op1.getY () == op2.getY ()) && 
	(op1.getZ () == op2.getZ ());
    
}


bool
operator!= ( const Point3D& op1, const Point3D& op2 )
{
    return !(op1 == op2);
}




std::ostream&
operator<< ( std::ostream& os, const Point3D& op )
{
    os << "(" << op.getX () << "," << op.getY () << "," << op.getZ () << ")";
    return os;
}



}
}

