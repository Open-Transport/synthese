#ifndef SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H
#define SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H

#include "15_env/Point.h"

#include <vector>

namespace synthese
{
namespace env
{
    class Point;
}

namespace carto
{

class DrawableLine;



class DrawableLineComparator
{
private:	

    const DrawableLine* _reference;
    const synthese::env::Point _referencePoint;
    
    synthese::env::Point _point;
    
    
    int firstIndexOf (const synthese::env::Point& point, 
      const std::vector<synthese::env::Point>& points) const;
	
public:

    DrawableLineComparator(const DrawableLine* reference, 
			   const synthese::env::Point& referencePoint, 
			   const synthese::env::Point& point);
    
    ~DrawableLineComparator();
	
    void setPoint (const synthese::env::Point& point);


    std::pair<double, double>
	calculateStartAngles (const std::vector<synthese::env::Point>& points1, int index1,
			      const std::vector<synthese::env::Point>& points2, int index2) const;

    std::pair<double, double>
	calculateEndAngles (const std::vector<synthese::env::Point>& points1, int index1,
			    const std::vector<synthese::env::Point>& points2, int index2) const;


	
    double calculateStartAngleAtIndex (
	const std::vector<synthese::env::Point>& points, 
	int index) const;
    
    double calculateEndAngleAtIndex (
	const std::vector<synthese::env::Point>& points, 
	int index) const;
	
    int operator() (const DrawableLine* bl1, const DrawableLine* bl2) const;
	
};

}
}

#endif
