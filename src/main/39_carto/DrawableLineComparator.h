#ifndef SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H
#define SYNTHESE_CARTO_DRAWABLELINECOMPARATOR_H


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
    const synthese::env::Point* _referencePoint;
    
    const synthese::env::Point* _point;
    
    
    int firstIndexOf (const synthese::env::Point* point, 
      const std::vector<const synthese::env::Point*>& points) const;
	
public:

    DrawableLineComparator(const DrawableLine* reference, 
			   const synthese::env::Point* referencePoint, 
			   const synthese::env::Point* point);
    
    ~DrawableLineComparator();
	
    void setPoint (const synthese::env::Point* point);
	
    double calculateStartAngleAtIndex (
	const std::vector<const synthese::env::Point*>& points, 
	int index) const;
    
    double calculateEndAngleAtIndex (
	const std::vector<const synthese::env::Point*>& points, 
	int index) const;
	
    int operator() (const DrawableLine* bl1, const DrawableLine* bl2) const;
	
};

}
}

#endif
