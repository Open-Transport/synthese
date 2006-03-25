#include "DrawableLineIndex.h"

#include "15_env/Point.h"


namespace synthese
{
namespace carto
{


DrawableLineIndex::DrawableLineIndex()
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

	

std::set<DrawableLine*>&
DrawableLineIndex::find (const synthese::env::Point& point) const
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
    find (point).insert (line);
}








}
}
