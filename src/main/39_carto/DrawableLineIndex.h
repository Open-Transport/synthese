#ifndef SYNTHESE_CARTO_DRAWABLELINEINDEX_H
#define SYNTHESE_CARTO_DRAWABLELINEINDEX_H

#include "15_env/Point.h"

#include <map>
#include <vector>
#include <set>

namespace synthese
{
namespace env
{
    class Point;
}

namespace carto
{

class DrawableLine;


class DrawableLineIndex
{

private:

    typedef std::map<double, std::set<DrawableLine*>* > Index1D;
    typedef std::map<double, Index1D* > Index2D;
	
    mutable Index2D _index;
    double _scaleX;
    double _scaleY;
    
    mutable std::vector<synthese::env::Point> _fuzzyPoints;

protected:

public:

    DrawableLineIndex();
    ~DrawableLineIndex();
	
    const std::set<DrawableLine*>&
	find (const synthese::env::Point& point) const;
    

	void setScaleX (double scaleX);
	void setScaleY (double scaleY);

    void add (const synthese::env::Point& point, 
	      DrawableLine* line) const;

    synthese::env::Point getFuzzyPoint (const synthese::env::Point& point) const;



private:

    std::set<DrawableLine*>&
	doFind (const synthese::env::Point& point) const;


};

}
}

#endif
