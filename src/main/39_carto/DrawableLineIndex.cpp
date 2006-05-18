#include "DrawableLineIndex.h"

#include "Geometry.h"

#include <iostream>

using synthese::env::Point;


namespace synthese
{
namespace carto
{


DrawableLineIndex::DrawableLineIndex(double minDistance)
: _minDistance (minDistance)
{

}
 


DrawableLineIndex::~DrawableLineIndex()
{
	
    for (Index2D::iterator it2 = _index.begin (); 
	 it2 != _index.end (); ++it2) 
    {
	Index1D* index1 = it2->second;		
	for (Index1D::iterator it1 = index1->begin (); 
	     it1 != index1->end (); ++it1) {
	    delete it1->second; 
	}
	delete index1;
    }
}





const std::set<DrawableLine*>&
DrawableLineIndex::find (const synthese::env::Point& point) const
{
    return doFind (point);
}
	

std::set<DrawableLine*>&
DrawableLineIndex::doFind (const synthese::env::Point& point) const
{
    Index1D* xmap = _index[point.getX ()];
    if (xmap == 0) {
	xmap = new Index1D ();
	_index[point.getX ()] = xmap;
    } 
	
    std::set<DrawableLine*>* dlset = (*xmap)[point.getY ()];
    if (dlset == 0) {
	dlset = new std::set<DrawableLine*> ();
	(*xmap)[point.getY ()] = dlset;
    }
	
    return *dlset; 	
}



void 
DrawableLineIndex::add (const synthese::env::Point& point, 
			DrawableLine* line) const
{
    doFind (point).insert (line);
}



Point 
DrawableLineIndex::getFuzzyPoint (const Point& point) const
{
	// Iterate on all stored points looking for one which is 
	// distant to point of less than minDistance.
	for (std::vector<Point>::iterator it = _fuzzyPoints.begin ();
		it != _fuzzyPoints.end (); ++it) {
	    double d = calculateDistance (point, *it);
		if (d <= _minDistance) return (*it);
	}
	_fuzzyPoints.push_back (point);
	return point;
}






}
}
