#ifndef SYNTHESE_CARTO_MAP_H
#define SYNTHESE_CARTO_MAP_H

#include <iostream>


#include "PostscriptCanvas.h"
#include "Rectangle.h"
#include "DrawableLineIndex.h"


#include <set>
#include <map>
#include <vector>



namespace synthese
{

namespace env
{
    class Point;
}

namespace carto
{

    class DrawableLine;
    class MapBackgroundManager;


class Map
{
private:
    Rectangle _realFrame;
    double _width;	
    double _height;	
    
    double _mapScaleX;
    double _mapScaleY;
    
    PostscriptCanvas _canvas;
    
    const MapBackgroundManager* _backgroundManager;
    
    std::set<DrawableLine*> _selectedLines;
    DrawableLineIndex _indexedLines;  //!< Drawable lines indexed by point.
    
    std::vector<DrawableLine*>
	findLinesSharingPoint (const std::set<DrawableLine*>& drawableLines,
			       const synthese::env::Point* point) const;
    
    std::pair<const synthese::env::Point*, int>
	findMostSharedPoint (const DrawableLine* drawableLine, 
			     const std::set<DrawableLine*>& exclusionList = 
			     std::set<DrawableLine*> ()) const;
    
    DrawableLine* findMostSharedLine (const std::set<DrawableLine*>& drawableLines, 
				      const std::set<DrawableLine*>& exclusionList) const;
    
    
    std::pair<const DrawableLine*, int>
	findLeftMostLine (const synthese::env::Point* vertex, 
			  const DrawableLine* reference, 
			  const std::set<DrawableLine*>& lines) const;
    

    std::pair<const DrawableLine*, int>
	findRightMostLine (const synthese::env::Point* vertex, 
			   const DrawableLine* reference, 
			   const std::set<DrawableLine*>& lines) const;
    
    
    void 
	assignShiftFactors (const DrawableLine* reference, 
			    const synthese::env::Point* referencePoint, 
			    DrawableLine* drawableLine, 
			    const std::set<DrawableLine*>& exclusionList);
    
    const DrawableLine*
	findBestAvailableReference (const DrawableLine* drawableLine, 
				    const std::vector<DrawableLine*>& lines) const;
    
    bool hasBackgroundManager () const;
    
    void prepareLines ();
    
    void prepare ();
    
    void dumpBackground ();
    void dumpLines ();
    
public:


    Map(std::ostream& output,
	const std::set<DrawableLine*> selectedLines,
	const Rectangle& realFrame, 
	double width, 
	double height,
        const MapBackgroundManager* backgroundManager = 0);
    
    virtual ~Map();
    
    synthese::env::Point toRealFrame (const synthese::env::Point& p);
    synthese::env::Point toOutputFrame (const synthese::env::Point& p);
	
    double getWidth () const { return _width; }
    double getHeight () const { return _height; }
    
    PostscriptCanvas& getCanvas () { return _canvas; }
	
    void dump ();
};

}
}

#endif 
