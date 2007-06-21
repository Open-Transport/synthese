#ifndef SYNTHESE_GEOMETRY_POINT2D_H
#define SYNTHESE_GEOMETRY_POINT2D_H


#include <iostream>



namespace synthese
{
namespace geometry
{



/** Point2D class
    @ingroup m06
*/
class Point2D
{
    private:

        double _x; 
        double _y; 

    public:

        Point2D (double x, double y);
        ~Point2D () {}


        //! @name Getters/Setters
        //@{
        double getX () const { return _x; }
	void setX (double x) { _x = x; }

        double getY () const { return _y; }
	void setY (double y) { _y = y; }
        //@}

};


bool operator== ( const Point2D& op1, const Point2D& op2 );
bool operator!= ( const Point2D& op1, const Point2D& op2 );

std::ostream& operator<< ( std::ostream& os, const Point2D& op );


}

}

#endif





