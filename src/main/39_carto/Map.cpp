#include "Map.h"

#include "Geometry.h"
#include "DrawableLineComparator.h"
#include "DrawableLine.h"

#include "MapBackground.h"
#include "MapBackgroundManager.h"

#include "PostscriptCanvas.h"


#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <cmath>

#include "15_env/Point.h"

using synthese::env::Point;
using namespace std;


namespace synthese
{
namespace carto
{



Map::Map(const std::set<DrawableLine*>& selectedLines,
	 const Rectangle& realFrame, 
         double width, 
         double height,
         const MapBackgroundManager* backgroundManager)
         
: _realFrame (realFrame)
, _selectedLines (selectedLines)
, _width (width)
, _height (height)
, _mapScaleX (_width / _realFrame.getWidth ())
, _mapScaleY (_height / _realFrame.getHeight ())
, _backgroundManager (backgroundManager)
{
    populateLineIndex (selectedLines);

}




Map::Map(const std::set<DrawableLine*>& selectedLines,
	 double width, 
	 double height,
	 const MapBackgroundManager* backgroundManager)
: _realFrame (0,0,0,0)
, _selectedLines (selectedLines)
, _width (width)
, _height (height)
, _mapScaleX (_width / _realFrame.getWidth ())
, _mapScaleY (_height / _realFrame.getHeight ())
, _backgroundManager (backgroundManager)
{
    populateLineIndex (selectedLines);

    
    // The real frame is deduced to fit selected lines points
    double lowerLeftLatitude = std::numeric_limits<double>::max ();
    double lowerLeftLongitude = std::numeric_limits<double>::max ();
    double upperRightLatitude = std::numeric_limits<double>::min ();
    double upperRightLongitude = std::numeric_limits<double>::min ();

    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
	 it != selectedLines.end ();
	 ++it)
    {
	const std::vector<const Point*>& points = (*it)->getPoints ();
	for (std::vector<const Point*>::const_iterator itp = points.begin ();
	     itp != points.end () ; ++itp)
	{
	    const Point* p = *itp;
	    if (p->getX () < lowerLeftLatitude) lowerLeftLatitude = p->getX ();
	    if (p->getY () < lowerLeftLongitude) lowerLeftLongitude = p->getY ();
	    if (p->getX () > upperRightLatitude) upperRightLatitude = p->getX ();
	    if (p->getY () > upperRightLongitude) upperRightLongitude = p->getY ();
	}
    }

    _realFrame = synthese::carto::Rectangle (lowerLeftLatitude,
					     lowerLeftLongitude,
					     upperRightLatitude - lowerLeftLatitude,
					     upperRightLongitude - lowerLeftLongitude);

}




void 
Map::populateLineIndex (const std::set<DrawableLine*>& selectedLines)
{
    // Populate line index (indexed by point).
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
	 it != selectedLines.end ();
	 ++it)
    {
	const std::vector<const Point*>& points = (*it)->getPoints ();
	for (std::vector<const Point*>::const_iterator itp = points.begin ();
	     itp != points.end () ; ++itp)
	{
	    _indexedLines.add (**itp, *it);
	}
    }


}



Map::~Map()
{
}


Point 
Map::toRealFrame (const Point& p)
{
    return Point (
        (p.getX () * _realFrame.getWidth()) / _width + _realFrame.getX(),
        (p.getY () * _realFrame.getHeight()) / _height + _realFrame.getY());
}



Point 
Map::toOutputFrame (const Point& p)
{
    return Point (
	((p.getX() - _realFrame.getX()) / _realFrame.getWidth ()) * _width,
	((p.getY() - _realFrame.getY()) / _realFrame.getHeight()) * _height
	);
}






std::vector<DrawableLine*>
Map::findLinesSharingPoint (const std::set<DrawableLine*>& drawableLines,
			    const Point* point) const 
{
    std::vector<DrawableLine*> sharingLines;
    std::set<DrawableLine*> sharingSelectedLines = _indexedLines.find (*point);
    
    for (std::set<DrawableLine*>::const_iterator it = drawableLines.begin();
	 it != drawableLines.end (); ++it) {
	DrawableLine* dbl = *it;
	if (sharingSelectedLines.find (dbl) != sharingSelectedLines.end ())
	{
	    sharingLines.push_back (dbl);
	}
    }
    return sharingLines;
		
}							 	



std::pair<const Point*, int>
Map::findMostSharedPoint (const DrawableLine* line, 
			  const std::set<DrawableLine*>& exclusionList) const {
    int curPointIndex = -1;
    unsigned int cpt = 1;
	
    const std::vector<const Point*>& points = line->getPoints ();
    for (unsigned int i=0; i<points.size (); ++i) 
    {
	std::set<DrawableLine*> lines =
	    _indexedLines.find (*points[i]);
	
	// Filter lines
	for (std::set<DrawableLine*>::const_iterator it = exclusionList.begin ();
	     it != exclusionList.end (); ++it) {
	    if (lines.find (*it) != lines.end ()) lines.erase (*it);
	}

	if (lines.size () >= cpt) {
	    cpt = lines.size ();
	    curPointIndex = i;
	}
    }
    return std::pair<const Point*, int> (points[curPointIndex], cpt);
}





DrawableLine*
Map::findMostSharedLine (const std::set<DrawableLine*>& drawableLines, 
			 const std::set<DrawableLine*>& exclusionList) const
{
    std::pair<const Point*, int> curPointCpt	(0, -1);
	
    DrawableLine* dbl;
	
    int maxPoints = 0;
	
    for (std::set<DrawableLine*>::const_iterator it = drawableLines.begin ();
	 it != drawableLines.end (); ++it) {
	DrawableLine* line = *it;

	if (exclusionList.find (line) != exclusionList.end ()) continue;
		
	int nbSharedPoints = 0;
		
	// Calculate the total of shared points of this line
	for (int i=0; i<line->getPoints().size (); ++i) 
	{
	    nbSharedPoints += findLinesSharingPoint (
		drawableLines, 
		line->getPoints()[i]).size ();
	}
	if (nbSharedPoints > maxPoints) {
	    maxPoints = nbSharedPoints;
	    dbl = line;
	}
		
    }
    return dbl;
}







std::pair<const DrawableLine*, int>
Map::findLeftMostLine (const Point* v, const DrawableLine* reference, const std::set<DrawableLine*>& lines) const
{
    int leftMostShift = -10000;
    const DrawableLine* leftMostLine = 0;
	
    for (std::set<DrawableLine*>::const_iterator iter = lines.begin ();
	 iter != lines.end (); ++iter) {
	const DrawableLine* dbl = *iter;
		
	if (dbl->hasPoint(v) == false) continue;
		
	bool reverse = reference->isReverseWayAt (v, dbl);
		
	int indexPoint = dbl->firstIndexOf (v);
		
	if (dbl->isShifted (indexPoint)) {
	    int curShift = dbl->getShift(indexPoint);
	    if (reverse) curShift = -curShift;
			
	    if (curShift >leftMostShift) {
		leftMostShift = curShift;
		leftMostLine = dbl;
	    }
	}
    }
    return std::pair<const DrawableLine*, int> (leftMostLine, leftMostShift);
}






std::pair<const DrawableLine*, int>
Map::findRightMostLine (const Point* v, const DrawableLine* reference, const std::set<DrawableLine*>& lines) const
{
    int rightMostShift = +10000;
    const DrawableLine* rightMostLine = 0;
	
    for (std::set<DrawableLine*>::const_iterator iter = lines.begin ();
	 iter != lines.end (); ++iter) {
	const DrawableLine* dbl = *iter;
		
	if (dbl->hasPoint(v) == false) continue;
		
	bool reverse = reference->isReverseWayAt(v, dbl);
		
	int indexPoint = dbl->firstIndexOf(v);
	if (dbl->isShifted (indexPoint)) {
	    int curShift = dbl->getShift(indexPoint);
	    if (reverse) curShift = -curShift;
			
	    if (curShift < rightMostShift) {
		rightMostShift = curShift;
		rightMostLine = dbl;
	    } 
	}
    }
    return std::pair<const DrawableLine*, int> (rightMostLine, rightMostShift);
	
}



void 
Map::assignShiftFactors (const DrawableLine* reference, 
			 const Point* referencePoint, 
			 DrawableLine* line, 
			 const std::set<DrawableLine*>& exclusionList)
{
    // Check if the line is already in the exclusion list; if it is the 
    // case avoid re-processing it.
    if (exclusionList.find (line) != exclusionList.end ()) return;
	
    DrawableLineComparator cmp (reference, referencePoint, 0);
	
    // cout << endl<< "******************* Processing line " << line->getLine ()->getId () << endl;
    for (unsigned int j=0; j<line->getPoints().size (); ++j) {
	// cout << "Processing point " << j << endl;
		
	const Point* v = line->getPoints()[j];
		
	// Looks in the exclusion for the right most line to this point 
	// and the left most line to this point.
	std::pair<const DrawableLine*, int> leftMostLineAndShift  = 
	    findLeftMostLine  (v, reference, exclusionList);
			
	std::pair<const DrawableLine*, int> rightMostLineAndShift = 
	    findRightMostLine (v, reference, exclusionList);
		
	const DrawableLine* leftMostLine = leftMostLineAndShift.first;
	const DrawableLine* rightMostLine = rightMostLineAndShift.first;

	int leftMostShift = leftMostLineAndShift.second;
	int rightMostShift = rightMostLineAndShift.second;
		
	cmp.setPoint (v);
		
	if ((rightMostLine == 0) && (leftMostLine == 0)) 
	{
	    line->setShift (j, 0);
	} 
	else 
	{
			
	    // cout << "=== LeftMostLine  = " << leftMostLine->getLineNumber() << " lms=" << leftMostShift <<endl;
	    // cout << "=== RightMostLine = " << rightMostLine->getLineNumber() << " rms=" << rightMostShift <<endl;
		
	    if (cmp (line, leftMostLine) == 0) {
		int newShift = leftMostShift + 1;
		if (line->isReverseWayAt(referencePoint, reference)) newShift = -newShift;
		line->setShift (j, newShift); 
				
	    } else if (cmp (line, rightMostLine) > 0) { 
		int newShift = rightMostShift - 1;
		if (line->isReverseWayAt(referencePoint, reference)) newShift = -newShift;
		line->setShift (j, newShift); 
	    }

	}
				
    }
}




const DrawableLine*
Map::findBestAvailableReference (const DrawableLine* line, 
				 const std::vector<DrawableLine*>& lines) const {
    // This function tries to find the best reference for line among lines.
    // Lines are already processed, that is why they have to a reference whenever
    // the most shared bus line joins one of them.
    // The best candidate for being a reference is the one which shares the most points
    // with line.
	
    const DrawableLine* currentReference = 0;
    int currentNumberOfCommonPoints = 0;
    for (unsigned int i=0; i<lines.size (); ++i) {
	int numberOfCommonPoints = line->numberOfCommonPointsWith(lines[i]);
	if (numberOfCommonPoints > currentNumberOfCommonPoints) {
	    currentNumberOfCommonPoints = numberOfCommonPoints;
	    currentReference = lines[i];
	}
    }				 	
								 	
    if (currentReference == 0) {
	// If lines was empty or if no point was shared the best reference is
	// line itself.
	return line;	
    }
	
    return currentReference;
}



void 
Map::prepareLines ()
{
    std::set<DrawableLine*> drawableLines (_selectedLines);
    std::set<DrawableLine*> exclusionList;

    while (exclusionList.size () < drawableLines.size ()) {
	// Do the following until all the lines have been processed
	// (as soon a line is processed it is in the exclusion list).
	// Even if a line is in the exclusion list, it does NOT mean that it
	// cannot participate to futher comparisons. It just means that it
	// should not be processed another time.
		
	// Get the most shared bus line	
	DrawableLine* mostSharedLine = findMostSharedLine (drawableLines, exclusionList);
		
	// cout <<  endl << endl << "%%%%% Most Shared Bus line = " << mostSharedLine->getLineNumber () << endl;
		
	// Get the most shared point of the most shared bus line
	// TODO : check there cannot be an infinite loop going through this.
	std::pair<const Point*, int> pointAndCpt = findMostSharedPoint (mostSharedLine);

	// Check if the most shared bus line is sharing its most shared point
	// with a line which is in the exclusion list. If yes, this line becomes
	// the reference; otherwiese, the reference is the most shared line itself.
	std::vector<DrawableLine*> lines = findLinesSharingPoint(exclusionList, pointAndCpt.first);
	// cout << "lines.size ()=" << lines.size () << " point = " << pointAndCpt.first->getX () << ";" << pointAndCpt.first->getY ()<< endl;

	const DrawableLine* reference = 
	    findBestAvailableReference (mostSharedLine, lines); 

	// Get all the lines sharing the most shared point of the most shared line
	// including the most shared line itself.
	std::vector<DrawableLine*> sharingLines = 
	    findLinesSharingPoint (drawableLines, pointAndCpt.first);

	// Sort the sharing lines according to their 'begin' and 'end' angle 
	// at sharing point.
	// NOTE : it is essential that the comparator used here shares the same
	// reference line and reference point with the one that is used
	// further for assigning shift factors.
	DrawableLineComparator cmp (reference, pointAndCpt.first, pointAndCpt.first);
	stable_sort (sharingLines.begin (), sharingLines.end (), cmp);

	// cout << "Processing order (ref=" << reference->getLineNumber () << "): ";
	// for (int l=0; l<sharingLines.size (); ++l) cout << sharingLines[l]->getLineNumber () << " ; ";
	// cout << endl;

	// Find back the reference line index in the sorted list.
	int referenceIndex;
	for (unsigned int i=0; i<sharingLines.size (); ++i) {
	    if (sharingLines[i] == reference) {
		referenceIndex = i;
		break;
	    }
	}
		
	// Now first iterate from referenceIndex (included) towards 0 ...
	for (int i=referenceIndex; i>=0; --i) {
	    assignShiftFactors (reference, pointAndCpt.first, sharingLines[i], exclusionList);
	    exclusionList.insert (sharingLines[i]);
	}
	// cout << "--------------------------------------" << endl;
		
	// ... and from referenceIndex (excluded) towards the end of the list
	if (referenceIndex < (int) sharingLines.size ()-1) {
	    for (int i=referenceIndex+1; i< (int) sharingLines.size (); ++i) {
		assignShiftFactors (reference, pointAndCpt.first, sharingLines[i], exclusionList);
		exclusionList.insert (sharingLines[i]);
	    }
	}
	
    }

}




bool 
Map::hasBackgroundManager () const
{
    return _backgroundManager != 0;   
}



void 
Map::prepare () 
{
    prepareLines ();
}


void 
Map::dumpBackground (PostscriptCanvas& canvas)
{
    if (hasBackgroundManager ()) 
    {
        const MapBackground* mbg = _backgroundManager->getBestScalingBackground (_mapScaleX, _mapScaleY);
        if (mbg != 0) {
            // Draw background
            std::pair<int,int>  tlIndexes = mbg->getIndexesOfTileContaining (_realFrame.getX(), _realFrame.getY ());
            std::pair<int,int>  brIndexes = mbg->getIndexesOfTileContaining (
		_realFrame.getX () + _realFrame.getWidth (), 
		_realFrame.getY () + _realFrame.getHeight ());
            
	    // TODO : additional checks in case indexes are negative/out of frame.

            int nbtiles = 0;
            for (int i=tlIndexes.first; i<=brIndexes.first; ++i) {
                for (int j=tlIndexes.second; j<=brIndexes.second; ++j) {
                    const MapBackgroundTile* tile = mbg->getTile (i, j);
                    if (tile != 0) { // Any background available for this tile ?
                        // cout << "Drawing tile " << i << "," << j <<  "  "<< tile->getPath ().string () << endl;
                        ++nbtiles;
                        tile->draw (*this, canvas);
                    }
                }
            }
        }
    }
    
    
}



void 
Map::dumpLines (PostscriptCanvas& canvas)
{
    // Draw drawableLines
    canvas.setfont("Helvetica", 10);
    canvas.setlinejoin (1);
    
    for (std::set<DrawableLine*>::const_iterator it = _selectedLines.begin ();
         it != _selectedLines.end () ; ++it) {
	const DrawableLine* dbl = *it;
	dbl->preDraw (*this, canvas);  
    }    
    for (std::set<DrawableLine*>::const_iterator it = _selectedLines.begin ();
         it != _selectedLines.end () ; ++it) {
	const DrawableLine* dbl = *it;
	dbl->draw (*this, canvas);  
    }    
    for (std::set<DrawableLine*>::const_iterator it = _selectedLines.begin ();
         it != _selectedLines.end () ; ++it) {
	const DrawableLine* dbl = *it;
	dbl->postDraw (*this, canvas);  
    }    
    
}






void 
Map::dump (PostscriptCanvas& canvas)
{
    prepare ();
	
    canvas.startPage(0, 0, _width, _height);
    
    dumpBackground (canvas);
    dumpLines (canvas);


//	canvas.setRGBColor(c0.r, c0.g, c0.b);
	
/*	// Draw streets in two steps
	for (unsigned int i=0; i<_streets.size(); ++i) {
	drawStreet (_streets[i], canvas);	
	}
	
	canvas.setLineWidth (3);
//	canvas.setLineWidth (3);
canvas.setRGBColor(c1.r, c1.g, c1.b);
	
for (unsigned int i=0; i<_streets.size(); ++i) {
drawStreet (_streets[i], canvas);	
}
	
*/
    /*
      canvas.setRGBColor(c0.r, c0.g, c0.b);
      canvas.setFont("Helvetica", 10);
      for (unsigned int i=0; i<_streets.size(); ++i) {
      drawText (_streets[i], 0.0, "Rue des arbres", canvas);
      } 
    */
	
/*	canvas.newPath ();
	canvas.moveTo (200.0, 200.0);
	canvas.lineTo (200.0, 100.0);
	canvas.lineTo (300.0, 300.0);
	canvas.pathtext(50.0, "Salut merdeSalut merdeSalut merde");
*/
	
	
    canvas.showPage();
	
	
}




double 
Map::getWidth () const 
{ 
    return _width; 
}




double 
Map::getHeight () const 
{ 
    return _height; 
}









}
}


