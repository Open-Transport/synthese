#ifndef SYNTHESE_CARTO_MAP_H
#define SYNTHESE_CARTO_MAP_H

#include <iostream>


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
    class DrawablePhysicalStop;
    class MapBackgroundManager;
    class PostscriptCanvas;


class Map
{
public:

private:

    Rectangle _realFrame;
    double _width;	
    double _height;	
    double _neighborhood;

	bool _preserveRatio;

    double _mapScaleX;
    double _mapScaleY;

	bool _lineGrouping;
    
    const MapBackgroundManager* _backgroundManager;
    const std::string _urlPattern;
    
    std::set<DrawableLine*> _selectedLines;
    std::set<DrawablePhysicalStop*> _selectedPhysicalStops;

    DrawableLineIndex _indexedLines;  //!< Drawable lines indexed by point.
    
    std::vector<DrawableLine*>
	findLinesSharingPoint (const std::set<DrawableLine*>& drawableLines,
			       const synthese::env::Point& point) const;
    
    std::pair<synthese::env::Point, int>
	findMostSharedPoint (const DrawableLine* drawableLine, 
			     const std::set<DrawableLine*>& exclusionList = 
			     std::set<DrawableLine*> ()) const;
    
    DrawableLine* findMostSharedLine (const std::set<DrawableLine*>& drawableLines, 
				      const std::set<DrawableLine*>& exclusionList) const;
    
    
    std::pair<const DrawableLine*, int>
	findLeftMostLine (const synthese::env::Point& vertex, 
			  const DrawableLine* reference, 
			  const std::set<DrawableLine*>& lines) const;
    

    std::pair<const DrawableLine*, int>
	findRightMostLine (const synthese::env::Point& vertex, 
			   const DrawableLine* reference, 
			   const std::set<DrawableLine*>& lines) const;
    
    
    void 
	assignShiftFactors (const DrawableLine* reference, 
			    const synthese::env::Point& referencePoint, 
			    DrawableLine* drawableLine, 
			    const std::set<DrawableLine*>& exclusionList);
    
    const DrawableLine*
	findBestAvailableReference (const DrawableLine* drawableLine, 
				    const std::vector<DrawableLine*>& lines) const;
    
    
    
    static void populateLineIndex (const DrawableLineIndex& lineIndex, const std::set<DrawableLine*>& selectedLines);
    void prepareLines ();
    void preparePhysicalStops ();
    void dumpBackground (PostscriptCanvas& canvas);
    void dumpLines (PostscriptCanvas& canvas);
    void dumpPhysicalStops (PostscriptCanvas& canvas);
    
public:


    Map(const std::set<DrawableLine*>& selectedLines,
	const Rectangle& realFrame, 
	double width, 
	double height,
	bool preserveRatio,
        const MapBackgroundManager* backgroundManager = 0,
	const std::string& urlPattern = "");


    Map(const std::set<DrawableLine*>& selectedLines,
	double width, 
	double height,
	double neighborhood,
	bool preserveRatio,
    const MapBackgroundManager* backgroundManager = 0,
	const std::string& urlPattern = "");

    
    virtual ~Map();
    
    synthese::env::Point toRealFrame (const synthese::env::Point& p);
    synthese::env::Point toOutputFrame (const synthese::env::Point& p);
	
    //! @name Getters/Setters
    //@{

    Rectangle getRealFrame () const;
    Rectangle getOutputFrame () const;

    double getWidth () const;
    double getHeight () const;

    double getScaleX () const;
    double getScaleY () const;

    bool hasBackgroundManager () const;
    const MapBackgroundManager* getBackgroundManager () const;

    const std::string& getUrlPattern () const;

    const std::set<DrawableLine*>& getSelectedLines () const;
    const std::set<DrawablePhysicalStop*>& getSelectedPhysicalStops () const;
    
	bool getLineGrouping () const;
	void setLineGrouping (bool lineGrouping);

	const DrawableLineIndex& getLineIndex () const;

    //@}

    //! @name Update methods
    //@{
    void prepare ();
    //@}

};

}
}

#endif 

