#ifndef SYNTHESE_GEOGRAPHY_GEOPOINT_H
#define SYNTHESE_GEOGRAPHY_GEOPOINT_H


#include <iostream>



namespace synthese
{
namespace geography
{



/** GeoPoint class.
    Geodetic point, based on WGS84 System (used by GPSs), usually expressed as 
    latitude, longitude, and ellipsoid height (height above WGS84 reference ellipsoid -
    which is GRS80).
    

    Check :
    http://www.mentorsoftwareinc.com/resource/glossary.htm
    http://en.wikipedia.org/wiki/WGS84
    http://en.wikipedia.org/wiki/GRS_80
    
    @ingroup m14
*/
class GeoPoint
{
    private:

    double _latitude;             // Latitude in degrees.
        double _longitude;        // Longitude in degrees.
        double _ellipsoidHeight;  // Height above ellipsoid in meters.

    public:

        GeoPoint (double latitude, double longitude, double ellipsoidHeight);
        ~GeoPoint () {}


        //! @name Getters/Setters
        //@{
	/** Latitude (phi)
	 */
        double getLatitude () const { return _latitude; }
	void setLatitude (double latitude) { _latitude = latitude; }

	/** Longitude (lambda)
	 */
        double getLongitude () const { return _longitude; }
	void setLongitude (double longitude) { _longitude = longitude; }

	/** Height above GRS_80 ellipsoid (he).
	    This is NOT the commonly known concept of altitude 
	    (even if it could be a very rough approximation +-100m) !  
	 */
        double getEllipsoidHeight () const { return _ellipsoidHeight; }
	void setEllipsoidHeight (double ellipsoidHeight) { _ellipsoidHeight = ellipsoidHeight; }

        //@}

};


bool operator== ( const GeoPoint& op1, const GeoPoint& op2 );
bool operator!= ( const GeoPoint& op1, const GeoPoint& op2 );

std::ostream& operator<< ( std::ostream& os, const GeoPoint& op );


}

}

#endif





