#include "Map.h"

#include "Geometry.h"
#include "DrawableLineComparator.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "MapBackground.h"
#include "MapBackgroundManager.h"

#include "PostscriptCanvas.h"
#include "01_util/RGBColor.h"



#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <cmath>
#include <limits>

#include "15_env/Point.h"
#include "15_env/PhysicalStop.h"


#include "01_util/Log.h"
#include "01_util/Conversion.h"

using synthese::util::Log;
using synthese::util::Conversion;


using synthese::env::Point;
using synthese::env::PhysicalStop;
using namespace std;


namespace synthese
{
namespace carto
{



Map::Map(const std::set<DrawableLine*>& selectedLines,
	 const Rectangle& realFrame, 
         double width, 
         double height,
         const MapBackgroundManager* backgroundManager,
	 const std::string& urlPattern)
    
: _realFrame (realFrame)
, _selectedLines (selectedLines)
, _width (width)
, _height (height)
, _mapScaleX (_width / _realFrame.getWidth ())
, _mapScaleY (_height / _realFrame.getHeight ())
, _backgroundManager (backgroundManager)
, _urlPattern (urlPattern)
, _horizontalMargin (0)
, _verticalMargin (0)
, _lineGrouping (true)

{
	_indexedLines.setScaleX(_mapScaleX);
	_indexedLines.setScaleY(_mapScaleY);
    populateLineIndex (_indexedLines, _selectedLines);

}




Map::Map(const std::set<DrawableLine*>& selectedLines,
	 double width, 
	 double height,
	 const MapBackgroundManager* backgroundManager,
	 const std::string& urlPattern)

: _realFrame (0,0,0,0)
, _selectedLines (selectedLines)
, _width (width)
, _height (height)
, _mapScaleX (_width / _realFrame.getWidth ())
, _mapScaleY (_height / _realFrame.getHeight ())
, _backgroundManager (backgroundManager)
, _urlPattern (urlPattern)
, _horizontalMargin (0)
, _verticalMargin (0)
, _lineGrouping (true)
{
    
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
  if ((_width == -1) && (_height == -1)) _width = 400;
  if (_width == -1) {
	  _width = _height * _realFrame.getWidth () / _realFrame.getHeight ();
  } else if (_height == -1) {
	  _height = _width * _realFrame.getHeight () / _realFrame.getWidth ();
  }
  _mapScaleX = _width / _realFrame.getWidth ();
  _mapScaleY = _height / _realFrame.getHeight ();

	_indexedLines.setScaleX(_mapScaleX);
	_indexedLines.setScaleY(_mapScaleY);

    populateLineIndex (_indexedLines, _selectedLines);


}




void 
Map::populateLineIndex (const DrawableLineIndex& lineIndex, const std::set<DrawableLine*>& selectedLines)
{
	
    // Populate line index (indexed by point).
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
	 it != selectedLines.end ();
	 ++it)
    {
		(*it)->fuzzyfyPoints (lineIndex);

		const std::vector<Point>& points = (*it)->getFuzzyfiedPoints ();
		for (std::vector<Point>::const_iterator itp = points.begin ();
			itp != points.end () ; ++itp)
		{
			lineIndex.add (*itp, *it);
		}

	}



}



Map::~Map()
{
    // Delete drawable lines
    for (std::set<DrawableLine*>::const_iterator it = _selectedLines.begin ();
	 it != _selectedLines.end (); ++it) 
    {	
		//std::cerr << "Deleting " << ((long) (*it)) << std::endl;
	delete (*it);
    }

    // Delete drawable physical stops
    for (std::set<DrawablePhysicalStop*>::const_iterator it = 
        _selectedPhysicalStops.begin (); it != _selectedPhysicalStops.end ();
        ++it)
    {
	delete (*it);
    }

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
    double widthWithoutMargin = _width - _horizontalMargin;
    double heightWithoutMargin = _height - _verticalMargin;

    return Point (
	((p.getX() - _realFrame.getX()) / _realFrame.getWidth ()) * widthWithoutMargin 
            + _horizontalMargin / 2,
	((p.getY() - _realFrame.getY()) / _realFrame.getHeight()) * heightWithoutMargin 
            + _verticalMargin / 2
	);
}




const std::string& 
Map::getUrlPattern () const
{
    return _urlPattern;
}




int 
Map::getHorizontalMargin () const
{
    return _horizontalMargin;
}


void 
Map::setHorizontalMargin (int horizintalMargin)
{
    _horizontalMargin = horizintalMargin;
}



int 
Map::getVerticalMargin () const
{
    return _verticalMargin;
}



void 
Map::setVerticalMargin (int verticalMargin)
{
    _verticalMargin = verticalMargin;
}




std::vector<DrawableLine*>
Map::findLinesSharingPoint (const std::set<DrawableLine*>& drawableLines,
			    const Point& point) const 
{
	// std::cerr << "*** findLinesSharingPoint " << drawableLines.size () << " x="
	// 	<< point.getX () << " y=" << point.getY () << std::endl;
	if (drawableLines.size () == 0) {
		int a = 0;
	}

    std::vector<DrawableLine*> sharingLines;
    const std::set<DrawableLine*>& sharingSelectedLines = _indexedLines.find (point);
    
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



std::pair<Point, int>
Map::findMostSharedPoint (const DrawableLine* line, 
			  const std::set<DrawableLine*>& exclusionList) const {
    int curPointIndex = -1;
    unsigned int cpt = 1;
	
    const std::vector<Point>& points = line->getFuzzyfiedPoints ();
    for (unsigned int i=0; i<points.size (); ++i) 
    {
	// Create a copy 
	std::set<DrawableLine*> lines =
	    _indexedLines.find (points[i]);
	
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
    return std::pair<Point, int> (points[curPointIndex], cpt);
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
		for (int i=0; i<line->getFuzzyfiedPoints().size (); ++i) 
		{
			nbSharedPoints += findLinesSharingPoint (
			drawableLines, 
			line->getFuzzyfiedPoints()[i]).size ();
		}
		if (nbSharedPoints > maxPoints) {
			maxPoints = nbSharedPoints;
			dbl = line;
		}
			
    }
    return dbl;
}







std::pair<const DrawableLine*, int>
Map::findLeftMostLine (const Point& v, const DrawableLine* reference, const std::set<DrawableLine*>& lines) const
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
Map::findRightMostLine (const Point& v, const DrawableLine* reference, const std::set<DrawableLine*>& lines) const
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
			 const Point& referencePoint, 
			 DrawableLine* line, 
			 const std::set<DrawableLine*>& exclusionList)
{
    // Check if the line is already in the exclusion list; if it is the 
    // case avoid re-processing it.
    if (exclusionList.find (line) != exclusionList.end ()) return;
	
    DrawableLineComparator cmp (reference, referencePoint, 0);
	
    // cout << endl<< "******************* Processing line " << line->getShortName () << endl;
    for (unsigned int j=0; j<line->getFuzzyfiedPoints().size (); ++j) {
	// cout << "Processing point " << j << endl;
	
	const Point& v = line->getFuzzyfiedPoints()[j];
	
	if (_lineGrouping)
	{
	    // For the current point, check if there is another line with the
	    // same color which has been shifted for this point. If yes,
	    // re-apply the same shift, otherwise process it.
	    const std::set<DrawableLine*> sharingLines = 
		_indexedLines.find (v);
	    bool isGrouped (false);
	    for (std::set<DrawableLine*>::const_iterator it = sharingLines.begin();
		 (it != sharingLines.end()) && (!isGrouped); ++it) 
	    {
		const DrawableLine* itline = *it;
		
		int index = itline->firstIndexOf (v);
		if ((itline->getColor () == line->getColor ()) && 
		    (itline->isShifted (index))) 
		{
		    line->setShift (j, itline->getShift (index));
		    isGrouped = true;
		}
		
	    }
	    // On peut passer au point suivant si la ligne a été groupée
	    // par sa couleur
	    if (isGrouped) continue;
	}
	
	
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
			
	    // cout << "=== LeftMostLine  = " << leftMostLine->getShortName () << " lms=" << leftMostShift <<endl;
	    // cout << "=== RightMostLine = " << rightMostLine->getShortName() << " rms=" << rightMostShift <<endl;
		
	    if (cmp (line, leftMostLine) == 0) {
		// cout << "... left of leftMostLine" <<endl;
		int newShift = leftMostShift + 1;
		if (line->isReverseWayAt(referencePoint, reference)) newShift = -newShift;
		line->setShift (j, newShift); 
				
	    } else if (cmp (line, rightMostLine) > 0) { 
		// cout << "... right of rightMostLine" <<endl;
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
    // This function tries to find the best reference for line among other lines.
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
	// If lines was empty or if no point was shared, the best reference is
	// line itself.
	return line;	
    }
	
    return currentReference;
}




void 
Map::preparePhysicalStops () 
{
    std::set<const PhysicalStop*> iteratedStops;
	std::vector<Point> fuzzyStopPoints;

    // Create drawable physical stops (for each physical stop)
    for (std::set<DrawableLine*>::const_iterator it = _selectedLines.begin ();
         it != _selectedLines.end () ; ++it) 
    {
	    const DrawableLine* dbl = *it;
        if (dbl->getWithPhysicalStops () == false) continue;

	    const std::vector<const Point*>& points = dbl->getPoints ();
	    for (int i=0; i<points.size (); ++i)
	    {
	        const Point* p = points[i];

	        const PhysicalStop* physicalStop = dynamic_cast<const PhysicalStop*> (p);
	        if (physicalStop)
	        {
				Point fuzzyPoint (_indexedLines.getFuzzyPoint (*p));
                if ( (iteratedStops.find (physicalStop) == iteratedStops.end ()) &&
					 (find (fuzzyStopPoints.begin(), fuzzyStopPoints.end(), fuzzyPoint) == fuzzyStopPoints.end ()) )
                {
                    // Guarantees a physical stop is added only once as a 
                    // DrawablePhysicalStop.
                    iteratedStops.insert (physicalStop);

					// Guarantees that two physical stops merged by fuzzyfication
					// will be drawn only once.
					fuzzyStopPoints.push_back (fuzzyPoint);

					DrawablePhysicalStop* dps = new DrawablePhysicalStop (physicalStop);
					dps->prepare (*this);
                    _selectedPhysicalStops.insert (dps);

                }
            }
        }
    }
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
		
	// cout <<  endl << endl << "%%%%% Most Shared Bus line = " << mostSharedLine->getShortName () << endl;
		
	// Get the most shared point of the most shared bus line
	// TODO : check there cannot be an infinite loop going through this.
	std::pair<Point, int> pointAndCpt = findMostSharedPoint (mostSharedLine);

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


	// cout << "Processing order (ref=" << reference->getShortName () << "): ";
	// for (int l=0; l<sharingLines.size (); ++l) cout << sharingLines[l]->getShortName () << " ; ";
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
	// cout << "----------------- OTHER SIDE ---------------------" << endl;
		
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




const MapBackgroundManager* 
Map::getBackgroundManager () const
{
    return _backgroundManager;   
}



Rectangle 
Map::getRealFrame () const
{
    return _realFrame;
}


Rectangle 
Map::getOutputFrame () const
{
    return Rectangle (0, 0, _width, _height);
}



void 
Map::prepare () 
{
	if (_selectedLines.size () == 0) return;

	prepareLines ();
    preparePhysicalStops ();
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


double 
Map::getScaleX () const
{
    return _mapScaleX;
}


double 
Map::getScaleY () const
{
    return _mapScaleY;
}



const std::set<DrawableLine*>& 
Map::getSelectedLines () const
{
    return _selectedLines;
}



const std::set<DrawablePhysicalStop*>& 
Map::getSelectedPhysicalStops () const
{
    return _selectedPhysicalStops;
}


bool
Map::getLineGrouping () const
{
	return _lineGrouping;
}


void 
Map::setLineGrouping (bool lineGrouping)
{
	_lineGrouping = lineGrouping;
}



const DrawableLineIndex& 
Map::getLineIndex () const
{
	return _indexedLines;
}








}
}


