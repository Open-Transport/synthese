
/** DrawableLineIndex class implementation.
	@file DrawableLineIndex.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "DrawableLineIndex.h"

#include "Geometry.h"

#include <iostream>

namespace synthese
{
	using namespace geometry;

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
DrawableLineIndex::find (const Point2D& point) const
{
    return doFind (point);
}
	

std::set<DrawableLine*>&
DrawableLineIndex::doFind (const Point2D& point) const
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
DrawableLineIndex::add (const Point2D& point, 
			DrawableLine* line) const
{
    doFind (point).insert (line);
}



Point2D 
DrawableLineIndex::getFuzzyPoint (const Point2D& point) const
{
	// Iterate on all stored points looking for one which is 
	// distant to point of less than a minimum distance on the 
	// final rendered map.
	for (std::vector<Point2D>::iterator it = _fuzzyPoints.begin ();
		it != _fuzzyPoints.end (); ++it) {
		Point2D fuzzyOutputPoint (it->getX() * _scaleX, 
								it->getY() * _scaleY);
		Point2D outputPoint (point.getX() * _scaleX, 
						   point.getY() * _scaleY);
	    double d = calculateDistance (outputPoint, fuzzyOutputPoint);
		if (d <= 20) return (*it); // 5 pixels
	}
	_fuzzyPoints.push_back (point);
	return point;
}






}
}

