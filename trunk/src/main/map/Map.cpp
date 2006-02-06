#include "Map.h"


#include "Geometry.h"
#include "DBLComparator.h"
#include "DrawableBusLine.h"
#include "DrawableTerminusBoard.h"
#include "Itinerary.h"
#include "Topography.h"
#include "Location.h"
#include "Edge.h"
#include "Vertex.h"
#include "MapBackground.h"
#include "MapBackgroundManager.h"


#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <cmath>

using namespace std;



namespace synmap
{


Map::Map(Topography* topography, 
         std::ostream& output,
		 const Rectangle& realFrame, 
         double width, 
         double height,
         const MapBackgroundManager* backgroundManager)
         
: _topography (topography)
, _realFrame (realFrame)
, _width (width)
, _height (height)
, _mapScaleX (_width / _realFrame.getWidth ())
, _mapScaleY (_height / _realFrame.getHeight ())
, _canvas (output)
, _backgroundManager (backgroundManager)
{
    cout << "Map scale x " << _mapScaleX << endl;
    cout << "Map scale y " << _mapScaleY << endl;
    
}


Map::~Map()
{
}


XYPoint 
Map::toRealFrame (const XYPoint& p)
{
	return XYPoint (
        (p.getX () * _realFrame.getWidth()) / _width + _realFrame.getX(),
        (p.getY () * _realFrame.getHeight()) / _height + _realFrame.getY());
}


XYPoint 
Map::toOutputFrame (const XYPoint& p)
{
	return XYPoint (
		((p.getX() - _realFrame.getX()) / _realFrame.getWidth ()) * _width,
		((p.getY() - _realFrame.getY()) / _realFrame.getHeight()) * _height
		);
}



/*
 * 
 * 

void
Map::drawRegion (const Region& region, PostscriptCanvas& canvas)
{
	if (region.size() < 1) return;
	canvas.newPath();
	
	XYPoint p0 = toOutputFrame(region[0]);
	canvas.moveTo(p0.getX(), p0.getY());
	
	for (int i=1; i<region.size (); ++i) {
		XYPoint p = toOutputFrame(region[i]);
		canvas.lineTo(p.getX(), p.getY());		
	}
	canvas.lineTo (p0.getX(), p0.getY());
	canvas.stroke();	
		
}

RGBColor c0 (0.0, 0.0, 0.0);
RGBColor c1 (234, 231, 138);





void
Map::drawStreet (const Street& street, PostscriptCanvas& canvas)
{
	if (street.size() < 1) return;
	canvas.newPath();
	
	XYPoint p0 = toOutputFrame(street[0]);
	canvas.moveTo(p0.getX(), p0.getY());
	
	for (unsigned int i=1; i<street.size (); ++i) {
		XYPoint p = toOutputFrame(street[i]);
		canvas.lineTo(p.getX(), p.getY());		
	}
	canvas.stroke();	
		
}

	
void
Map::drawText (const Street& street, double offset, const std::string& text, PostscriptCanvas& canvas)
{
	if (street.size() < 1) return;
	// temporary...
	for (unsigned int i=0; i<street.size () ; ++i) {
		if (_realFrame.contains(street[i]) == false) return;
	}	
	
	canvas.newPath();
	
	XYPoint p0 = toOutputFrame(street[0]);
	canvas.moveTo(p0.getX(), p0.getY());

	for (unsigned int i=1; i<street.size (); ++i) {
		XYPoint p = toOutputFrame(street[i]);
		canvas.lineTo(p.getX(), p.getY());		
	}
	
	canvas.pathtext(offset, text);	
		
}


// TODO : operateur de crop pour filter les objets dont la bounding box intersecte
// le frame de la carte.

// TODO : filtrage des objets trop petits.

*/



std::vector<DrawableBusLine*>
Map::findBusLinesSharingVertex (const DrawableBusLineSet& busLines,
							 	const Vertex* vertex) const 
{
	std::vector<DrawableBusLine*>	sharingLines;
	std::vector<const Itinerary*> sharingItineraries;
	_topography->findItineraries(vertex, sharingItineraries); 
	
	for (DrawableBusLineSet::const_iterator it = busLines.begin();
		 it != busLines.end (); ++it) {
		DrawableBusLine* dbl = *it;
		for (unsigned int i=0; i<sharingItineraries.size (); ++i) {
			if (dbl->getItinerary() == sharingItineraries[i]) {
				sharingLines.push_back (dbl);
			}
		}
	}
	return 	sharingLines;
		
}							 	



std::pair<const Vertex*, int>
Map::findMostSharedVertex (const DrawableBusLine* busLine, 
						   const DrawableBusLineSet& exclusionList) const {
	int curVertexIndex = -1;
	unsigned int cpt = 1;
	
	std::vector<const Itinerary*> itineraries;
	const std::vector<const Vertex*>& vertices = busLine->getItinerary()->getVertices ();
	for (unsigned int i=0; i<vertices.size (); ++i) {
		itineraries.clear ();
		_topography->findItineraries(vertices[i], itineraries);
		
		// Filter itineraries
		std::vector<const Itinerary*> itineraries2;
		for (unsigned int j=0; j<itineraries.size (); ++j) {
			bool found = false;
			
			for (DrawableBusLineSet::const_iterator it = exclusionList.begin ();
				 it != exclusionList.end (); ++it) {
				const DrawableBusLine* dbl = *it;
				if (dbl->getItinerary () == itineraries[j]) {
					found = true;
				}
			}
			if (!found) {
				itineraries2.push_back(itineraries[j]);
			}
		}

		if (itineraries2.size () >= cpt) {
			cpt = itineraries2.size ();
			curVertexIndex = i;
		}
	}
	return std::pair<const Vertex*, int> (vertices[curVertexIndex], cpt);
}





DrawableBusLine*
Map::findMostSharedBusLine (const DrawableBusLineSet& busLines, 
						    const DrawableBusLineSet& exclusionList) const
{
	std::pair<const Vertex*, int> curVertexCpt	(0, -1);
	
	DrawableBusLine* dbl;
	
	int maxVertices = 0;
	
	for (DrawableBusLineSet::const_iterator it = busLines.begin ();
		 it != busLines.end (); ++it) {
		DrawableBusLine* busLine = *it;

		if (exclusionList.find (busLine) != exclusionList.end ()) continue;
		
		int nbSharedVertices = 0;
		
		// Calculate the total of shared vertices of this line
		for (unsigned int i=0; i<busLine->getItinerary()->getVertices().size (); ++i) {
			nbSharedVertices += findBusLinesSharingVertex (busLines, busLine->getItinerary()->getVertices()[i]).size ();
		}
		if (nbSharedVertices > maxVertices) {
			maxVertices = 	nbSharedVertices;
			dbl = busLine;
		}
		
	}
	return dbl;
}





std::pair<const DrawableBusLine*, int>
Map::findLeftMostLine (const Vertex* v, const DrawableBusLine* reference, const DrawableBusLineSet& lines) const
{
	int leftMostShift = -10000;
	const DrawableBusLine* leftMostLine = 0;
	
	for (DrawableBusLineSet::const_iterator iter = lines.begin ();
		 iter != lines.end (); ++iter) {
		const DrawableBusLine* dbl = *iter;
		
		if (dbl->getItinerary()->hasVertex(v) == false) continue;
		
		bool reverse = reference->isReverseWayAt (v, dbl);
		
		int indexVertex = dbl->getItinerary()->firstIndexOf(v);
		
		if (dbl->isShifted (indexVertex)) {
			int curShift = dbl->getShift(indexVertex);
			if (reverse) curShift = -curShift;
			
			if (curShift >leftMostShift) {
				leftMostShift = curShift;
				leftMostLine = dbl;
			}
		}
	}
	return std::pair<const DrawableBusLine*, int> (leftMostLine, leftMostShift);
}






std::pair<const DrawableBusLine*, int>
Map::findRightMostLine (const Vertex* v, const DrawableBusLine* reference, const DrawableBusLineSet& lines) const
{
	int rightMostShift = +10000;
	const DrawableBusLine* rightMostLine = 0;
	
	for (DrawableBusLineSet::const_iterator iter = lines.begin ();
		 iter != lines.end (); ++iter) {
		const DrawableBusLine* dbl = *iter;
		
		if (dbl->getItinerary()->hasVertex(v) == false) continue;
		
		bool reverse = reference->isReverseWayAt(v, dbl);
		
		int indexVertex = dbl->getItinerary()->firstIndexOf(v);
		if (dbl->isShifted (indexVertex)) {
			int curShift = dbl->getShift(indexVertex);
			if (reverse) curShift = -curShift;
			
			if (curShift < rightMostShift) {
				rightMostShift = curShift;
				rightMostLine = dbl;
			} 
		}
	}
	return std::pair<const DrawableBusLine*, int> (rightMostLine, rightMostShift);
	
}



void 
Map::assignShiftFactors (const DrawableBusLine* reference, 
						 const Vertex* referenceVertex, 
						 DrawableBusLine* busLine, 
						 const DrawableBusLineSet& exclusionList)
{
	// Check if the line is already in the exclusion list; if it is the 
	// case avoid re-processing it.
	if (exclusionList.find (busLine) != exclusionList.end ()) return;
	
	DBLComparator cmp (reference, referenceVertex, 0);
	
	cout << endl<< "******************* Processing line " << busLine->getLineNumber () << endl;
	for (unsigned int j=0; j<busLine->getItinerary()->getVertices().size (); ++j) {
		// cout << "Processing point " << j << endl;
		
		const Vertex* v = busLine->getItinerary()->getVertices()[j];
		
		// Looks in the exclusion for the right most line to this vertex 
		// and the left most line to this vertex.
		std::pair<const DrawableBusLine*, int> leftMostLineAndShift  = 
			findLeftMostLine  (v, reference, exclusionList);
			
		std::pair<const DrawableBusLine*, int> rightMostLineAndShift = 
			findRightMostLine (v, reference, exclusionList);
		
		const DrawableBusLine* leftMostLine = leftMostLineAndShift.first;
		const DrawableBusLine* rightMostLine = rightMostLineAndShift.first;

		int leftMostShift = leftMostLineAndShift.second;
		int rightMostShift = rightMostLineAndShift.second;
		
		cmp.setVertex (v);
		
		if ((rightMostLine == 0) && (leftMostLine == 0)) {
			busLine->setShift (j, 0);
		} else {
			
			// cout << "=== LeftMostLine  = " << leftMostLine->getLineNumber() << " lms=" << leftMostShift <<endl;
			// cout << "=== RightMostLine = " << rightMostLine->getLineNumber() << " rms=" << rightMostShift <<endl;
		
			if (cmp (busLine, leftMostLine) == 0) {
				int newShift = leftMostShift + 1;
				if (busLine->isReverseWayAt(referenceVertex, reference)) newShift = -newShift;
				busLine->setShift (j, newShift); 
				
			} else if (cmp (busLine, rightMostLine) > 0) { 
				int newShift = rightMostShift - 1;
				if (busLine->isReverseWayAt(referenceVertex, reference)) newShift = -newShift;
				busLine->setShift (j, newShift); 
			}

		}
				
	}
}




const DrawableBusLine*
Map::findBestAvailableReference (const DrawableBusLine* busLine, 
								 const std::vector<DrawableBusLine*>& lines) const {
	// This function tries to find the best reference for busLine among lines.
	// Lines are already processed, that is why they have to a reference whenever
	// the most shared bus line joins one of them.
	// The best candidate for being a reference is the one which shares the most vertices
	// with busLine.
	
	const DrawableBusLine* currentReference = 0;
	int currentNumberOfCommonVertices = 0;
	for (unsigned int i=0; i<lines.size (); ++i) {
		int numberOfCommonVertices = busLine->numberOfCommonVerticesWith(lines[i]);
		if (numberOfCommonVertices > currentNumberOfCommonVertices) {
			currentNumberOfCommonVertices = numberOfCommonVertices;
			currentReference = lines[i];
		}
	}				 	
								 	
	if (currentReference == 0) {
		// If lines was empty or if no vertex was shared the best reference is
		// busLine itself.
		return busLine;	
	}
	
	return currentReference;
}



void 
Map::prepareBusLines ()
{
	// Create all the DrawableBusLines for itineraries in topography
    _busLines.clear (); // Memory leak here...
	const map<int, const Itinerary*>& itineraries = _topography->getItineraries ();
    for (map<int, const Itinerary*>::const_iterator it = itineraries.begin ();
         it != itineraries.end ();
         ++it) {
        const Itinerary* itinerary = it->second;
		DrawableBusLine* dbl = new DrawableBusLine (itinerary);
		_busLines.insert (dbl);	
	}
	
	DrawableBusLineSet busLines (_busLines);
	DrawableBusLineSet exclusionList;

	while (exclusionList.size () < busLines.size ()) {
		// Do the following until all the lines have been processed
		// (as soon a line is processed it is in the exclusion list).
		// Even if a line is in the exclusion list, it does NOT mean that it
		// cannot participate to futher comparisons. It just means that it
		// should not be processed another time.
		
		// Get the most shared bus line	
		DrawableBusLine* mostSharedBusLine = findMostSharedBusLine (busLines, exclusionList);
		
		// cout <<  endl << endl << "%%%%% Most Shared Bus line = " << mostSharedBusLine->getLineNumber () << endl;
		
		// Get the most shared vertex of the most shared bus line
		// TODO : check there cannot be an infinite loop going through this.
		std::pair<const Vertex*, int> vertexAndCpt = findMostSharedVertex (mostSharedBusLine);

		// Check if the most shared bus line is sharing its most shared vertex
		// with a line which is in the exclusion list. If yes, this line becomes
		// the reference; otherwiese, the reference is the most shared line itself.
		std::vector<DrawableBusLine*> lines = findBusLinesSharingVertex(exclusionList, vertexAndCpt.first);
		// cout << "lines.size ()=" << lines.size () << " vertex = " << vertexAndCpt.first->getX () << ";" << vertexAndCpt.first->getY ()<< endl;

		const DrawableBusLine* reference = 
			findBestAvailableReference (mostSharedBusLine, lines); 

		// Get all the lines sharing the most shared vertex of the most shared line
		// including the most shared line itself.
		std::vector<DrawableBusLine*> sharingLines = 
			findBusLinesSharingVertex (busLines, vertexAndCpt.first);

		// Sort the sharing lines according to their 'begin' and 'end' angle 
		// at sharing point.
		// NOTE : it is essential that the comparator used here shares the same
		// reference line and reference vertex with the one that is used
		// further for assigning shift factors.
		DBLComparator cmp (reference, vertexAndCpt.first, vertexAndCpt.first);
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
			assignShiftFactors (reference, vertexAndCpt.first, sharingLines[i], exclusionList);
			exclusionList.insert (sharingLines[i]);
		}
		// cout << "--------------------------------------" << endl;
		
		// ... and from referenceIndex (excluded) towards the end of the list
		if (referenceIndex < (int) sharingLines.size ()-1) {
			for (int i=referenceIndex+1; i< (int) sharingLines.size (); ++i) {
				assignShiftFactors (reference, vertexAndCpt.first, sharingLines[i], exclusionList);
				exclusionList.insert (sharingLines[i]);
			}
		}
	
	}

}


DrawableTerminusBoard*
Map::getTerminusBoard (const Vertex* vertex)
{
    DrawableTerminusBoardMap::iterator it = _terminusBoards.find (vertex);
    if (it == _terminusBoards.end ())
    {
        DrawableTerminusBoard* board = new DrawableTerminusBoard ("toto", vertex);
        _terminusBoards.insert (std::make_pair (vertex, board));
        return board;
    }
    return it->second;
}

    


void
Map::prepareTerminusBoards ()
{
    for (DrawableBusLineSet::const_iterator it = _busLines.begin ();
         it != _busLines.end () ; ++it) {
        const DrawableBusLine* dbl = *it;
        
        // Add the drawabale busline start and end to the terminus
        const Vertex* firstVertex = dbl->getItinerary()->getFirstStep ()->getVertex ();
        const Vertex* lastVertex = dbl->getItinerary()->getLastStep ()->getVertex ();
        DrawableTerminusBoard* board = getTerminusBoard (firstVertex);
        board->addDrawableBusLine (dbl);
        board = getTerminusBoard (lastVertex);
        board->addDrawableBusLine (dbl);
        
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
	prepareBusLines ();
	prepareTerminusBoards ();
    
}


void 
Map::dumpBackground ()
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
            
            int nbtiles = 0;
            for (int i=tlIndexes.first; i<=brIndexes.first; ++i) {
                for (int j=tlIndexes.second; j<=brIndexes.second; ++j) {
                    const MapBackgroundTile* tile = mbg->getTile (i, j);
                    if (tile != 0) { // Any background available for this tile ?
                        // cout << "Drawing tile " << i << "," << j <<  "  "<< tile->getPath ().string () << endl;
                        ++nbtiles;
                        tile->draw (*this);
                    }
                }
            }
            // std::cout << "TL=" << tlIndexes.first << "," << tlIndexes.second << std::endl;
            // std::cout << "BR=" << brIndexes.first << "," << brIndexes.second << std::endl;
        }
    }
    
    
}



void 
Map::dumpBusLines ()
{
    // Draw busLines
    _canvas.setfont("Helvetica", 10);
    _canvas.setlinejoin (1);
    
    for (DrawableBusLineSet::const_iterator it = _busLines.begin ();
         it != _busLines.end () ; ++it) {
            const DrawableBusLine* dbl = *it;
            dbl->preDraw (*this);  
    }    
    for (DrawableBusLineSet::const_iterator it = _busLines.begin ();
         it != _busLines.end () ; ++it) {
            const DrawableBusLine* dbl = *it;
            dbl->draw (*this);  
    }    
    for (DrawableBusLineSet::const_iterator it = _busLines.begin ();
         it != _busLines.end () ; ++it) {
            const DrawableBusLine* dbl = *it;
            dbl->postDraw (*this);  
    }    
    
}


void 
Map::dumpTerminusBoards ()
{
    for (DrawableTerminusBoardMap::const_iterator it = _terminusBoards.begin ();
         it != _terminusBoards.end () ; ++it) {
            const DrawableTerminusBoard* dtb = it->second;
            dtb->preDraw (*this);  
    }    
    for (DrawableTerminusBoardMap::const_iterator it = _terminusBoards.begin ();
         it != _terminusBoards.end () ; ++it) {
            const DrawableTerminusBoard* dtb = it->second;
            dtb->draw (*this);  
    }    
    for (DrawableTerminusBoardMap::const_iterator it = _terminusBoards.begin ();
         it != _terminusBoards.end () ; ++it) {
            const DrawableTerminusBoard* dtb = it->second;
            dtb->postDraw (*this);  
    }    
}




void 
Map::dump ()
{
	prepare ();
	
	_canvas.startPage(0, 0, _width, _height);
    
    dumpBackground ();
    dumpBusLines ();
    dumpTerminusBoards ();


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
	
	
	_canvas.showPage();
	
	
}









}
