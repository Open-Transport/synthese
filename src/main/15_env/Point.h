#ifndef SYNTHESE_ENV_POINT_H
#define SYNTHESE_ENV_POINT_H


#include "module.h"


namespace synthese
{
namespace env
{


/** Lambert II geographical point.
 
Lambert II geographical coordinates are stored as two integers : 
one for kilometers part, the other for the meter part.

( \f$ x=x_{km}*1000+x_m \f$ et \f$ y=y_{km}*1000+y_m \f$)
 
The goal of this split is to fasten some calculation which do not need metric accuracy.
 @ingroup m15
*/
class Point
{
        friend class SquareDistance;

        double _xM;  //!< Accurate longitude in meters
        double _yM;  //!< Accurate latitude in meters

        short int _xKm; //!< Longitude in kilometers
        short int _yKm; //!< Latitude in kilometers

    public:

        Point(double x = UNKNOWN_VALUE,
	      double y = UNKNOWN_VALUE);

        virtual ~Point();

	//! @name Query methods.
	//@{
        bool isUnknown () const;
	//@}

	//! @name Getters/Setters
	//@{
        double getX () const;
        double getY () const;

        /** Longitude setter
	    @param x New longitude in meters
	*/
        void setX ( double x);

        /** Latitude setter
	    @param x New longitude in meters
	*/
        void setY ( double y);
	//@}


        /** Coordinates comparison operator.
	  @param op Point to test
	  @return true if meters coordinates are identical, 
	  false otherwise or if one coordinate is unknown.
	*/
        bool operator == ( const Point& ) const;

        /** Coordinates comparison operator.
	  @param op Point to test
	  @return false if meters coordinates are identical, 
	  true otherwise or if one coordinate is unknown.
	*/
        bool operator != ( const Point& ) const;

};



}
}


#endif
