#include "DrawableLineIndex.h"

#include "Geometry.h"

#include <iostream>

using synthese::env::Point;


namespace synthese
{
namespace map
{


DrawableLineIndex::DrawableLineIndex()
: _scaleX (0.0)
, _scaleY (0.0)
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


void 
DrawableLineIndex::setScaleX (double scaleX)
{
	_scaleX = scaleX;
}



void 
DrawableLineIndex::setScaleY (double scaleY)
{
	_scaleY = scaleY;
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
	// distant to point of less than a minimum distance on the 
	// final rendered map.
	for (std::vector<Point>::iterator it = _fuzzyPoints.begin ();
		it != _fuzzyPoints.end (); ++it) {
		Point fuzzyOutputPoint (it->getX() * _scaleX, 
								it->getY() * _scaleY);
		Point outputPoint (point.getX() * _scaleX, 
						   point.getY() * _scaleY);
	    double d = calculateDistance (outputPoint, fuzzyOutputPoint);
		if (d <= 20) return (*it); // 5 pixels
	}
	_fuzzyPoints.push_back (point);
	return point;
}






}
}

