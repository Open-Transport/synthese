#ifndef SYNTHESE_CARTO_DRAWABLELINEINDEX_H
#define SYNTHESE_CARTO_DRAWABLELINEINDEX_H

#include <map>
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

protected:

public:

    DrawableLineIndex();
    ~DrawableLineIndex();
	
    std::set<DrawableLine*>&
	find (const synthese::env::Point& point) const;
    
};

}
}

#endif
