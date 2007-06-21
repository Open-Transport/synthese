#include "GeoPoint.h"




namespace synthese
{
namespace geography
{



GeoPoint::GeoPoint (double latitude, double longitude, double ellipsoidHeight)
    : _latitude (latitude)
    , _longitude (longitude)
    , _ellipsoidHeight (ellipsoidHeight)
{
}



    

bool
operator== ( const GeoPoint& op1, const GeoPoint& op2 )
{
    return 
	(op1.getLatitude () == op2.getLatitude ()) && 
	(op1.getLongitude () == op2.getLongitude ()) && 
	(op1.getEllipsoidHeight () == op2.getEllipsoidHeight ());
    
}


bool
operator!= ( const GeoPoint& op1, const GeoPoint& op2 )
{
    return !(op1 == op2);
}




std::ostream&
operator<< ( std::ostream& os, const GeoPoint& op )
{
    os << "(" << op.getLatitude () << "," << op.getLongitude () << "," << op.getEllipsoidHeight () << ")";
    return os;
}



}
}

