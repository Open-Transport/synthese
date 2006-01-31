#ifndef MAP_H_
#define MAP_H_

#include <iostream>

#include "Geometry.h"
#include "Rectangle.h"
#include "XYPoint.h"


#include "DrawableBusLine.h"

#include "PostscriptCanvas.h"

#include <set>
#include <map>

using namespace std;

namespace synmap
{

class Topography;
class Edge;
class Vertex;
class MapBackgroundManager;
class DrawableTerminusBoard;


class Map
{
private:
    typedef set<DrawableBusLine*> DrawableBusLineSet;     
    typedef map<const Vertex*, DrawableTerminusBoard*> DrawableTerminusBoardMap;


	Topography* _topography;

	Rectangle _realFrame;
	double _width;	
	double _height;	
    
    double _mapScaleX;
    double _mapScaleY;

	PostscriptCanvas _canvas;
	
    const MapBackgroundManager* _backgroundManager;

    // Dump objects
    DrawableBusLineSet       _busLines;
    DrawableTerminusBoardMap _terminusBoards;
    // Dump objects



	std::vector<DrawableBusLine*>
	findBusLinesSharingVertex (const DrawableBusLineSet& busLines,
								 	const Vertex* vertex) const;


	std::pair<const Vertex*, int>
	findMostSharedVertex (const DrawableBusLine* busLine, 
					      const DrawableBusLineSet& exclusionList = DrawableBusLineSet ()) const;
	
	DrawableBusLine* findMostSharedBusLine (const DrawableBusLineSet& busLines, 
							   const DrawableBusLineSet& exclusionList) const;
							   

	std::pair<const DrawableBusLine*, int>
	findLeftMostLine (const Vertex* vertex, 
					  const DrawableBusLine* reference, 
					  const DrawableBusLineSet& lines) const;
	
	std::pair<const DrawableBusLine*, int>
	findRightMostLine (const Vertex* vertex, 
					   const DrawableBusLine* reference, 
		 			   const DrawableBusLineSet& lines) const;
	
				
	void 
	assignShiftFactors (const DrawableBusLine* reference, 
						const Vertex* referenceVertex, 
						DrawableBusLine* busLine, 
						const DrawableBusLineSet& exclusionList);
	
	const DrawableBusLine*
	findBestAvailableReference (const DrawableBusLine* busLine, 
									 const std::vector<DrawableBusLine*>& lines) const;
	
	DrawableTerminusBoard*
    getTerminusBoard (const Vertex* vertex);
    
    
    bool hasBackgroundManager () const;
    
    void prepareBusLines ();
    void prepareTerminusBoards ();
	
	void prepare ();
	
    void dumpBackground ();
    void dumpBusLines ();
    void dumpTerminusBoards ();
    
public:


	Map(Topography* topography,
		std::ostream& output,
		const Rectangle& realFrame, 
		double width, 
		double height,
        const MapBackgroundManager* backgroundManager = 0);
	
	virtual ~Map();
	
	XYPoint toRealFrame (const XYPoint& p);
	XYPoint toOutputFrame (const XYPoint& p);
	
    double getWidth () const { return _width; }
    double getHeight () const { return _height; }
    
	PostscriptCanvas& getCanvas () { return _canvas; }
	
	void dump ();
};

}

#endif /*MAP_H_*/
