#include "Point2D.h"




namespace synthese
{
namespace geometry
{



Point2D::Point2D (double x, double y)
    : _x (x)
    , _y (y)
{
}



    

bool
operator== ( const Point2D& op1, const Point2D& op2 )
{
    return (op1.getX () == op2.getX ()) && 
	(op1.getY () == op2.getY ());
    
}


bool
operator!= ( const Point2D& op1, const Point2D& op2 )
{
    return !(op1 == op2);
}




std::ostream&
operator<< ( std::ostream& os, const Point2D& op )
{
    os << "(" << op.getX () << "," << op.getY () << ")";
    return os;
}



}
}

