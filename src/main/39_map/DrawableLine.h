#ifndef SYNTHESE_CARTO_DRAWABLELINE_H
#define SYNTHESE_CARTO_DRAWABLELINE_H

#include <string>
#include <vector>


#include "Drawable.h"

#include "01_util/RGBColor.h"
#include "01_util/UId.h"
#include "15_env/Point.h"




namespace synthese
{
namespace env
{
    class Line;
}


namespace map
{

class DrawableLineIndex;


class DrawableLine 
{
private:


    const uid _lineId;

    std::vector<const synthese::env::Point*> _points;  //!< Reference line points
    const std::string _shortName;
    const synthese::util::RGBColor _color;
    const bool _withPhysicalStops;

    std::vector<int> _shifts;
    std::vector<bool> _shifted;
	
	std::vector<synthese::env::Point> _fuzzyfiedPoints;
    mutable std::vector<synthese::env::Point> _shiftedPoints;

	


public:

    typedef enum { NONE, SINGLE, DOUBLE } PointShiftingMode;
    
    DrawableLine (const synthese::env::Line* line,
		  int fromLineStopIndex,
		  int toLineStopIndex,
		  bool withPhysicalStops = true);


    DrawableLine (const uid& lineId,
		  const std::vector<const synthese::env::Point*>& points,
		  const std::string& shortName,
		  const synthese::util::RGBColor& color,
		  bool withPhysicalStops = true);

    ~DrawableLine();

    //! @name Getters/Setters
    //@{
    const uid& getLineId () const;

    const std::string& getShortName () const;
	const synthese::util::RGBColor& getColor () const;
    bool getWithPhysicalStops () const;

    const std::vector<const synthese::env::Point*>& 
	getPoints () const;

	const std::vector<synthese::env::Point>& 
	getFuzzyfiedPoints () const;


    bool hasPoint (const synthese::env::Point&) const;

    bool isStopPoint (int pointIndex) const;
    bool isViaPoint (int pointIndex) const;

    int isShifted (int pointIndex) const;
    int getShift (int pointIndex) const;
    void setShift (int pointIndex, int shift);

    const std::vector<synthese::env::Point>& getShiftedPoints () const;

    //@}

	void fuzzyfyPoints (const DrawableLineIndex& lineIndex);

    bool isFullyReverseWay (const DrawableLine* dbl) const;
    bool isFullySameWay (const DrawableLine* dbl) const;
    bool isReverseWayAt (const synthese::env::Point& p, const DrawableLine* dbl) const;
    int numberOfCommonPointsWith (const DrawableLine* dbl) const;

    virtual void prepare (Map& map, double spacing, PointShiftingMode shiftMode = DOUBLE) const;
    
    /** Find first point in this line points
	which is equal (by value) to a given point.
    */
    int firstIndexOf (const synthese::env::Point& p) const;

    const std::vector<synthese::env::Point> 
	calculateShiftedPoints (const std::vector<synthese::env::Point>& points, 
				double spacing, 
				PointShiftingMode shiftMode) const;

	const std::vector<synthese::env::Point>
	calculateAbsoluteShiftedPoints (const std::vector<synthese::env::Point>& points, 
										 double spacing) const;

private:


    synthese::env::Point  
	calculateSingleShiftedPoint (synthese::env::Point a, 
				     synthese::env::Point b, 
				     double distance) const;

    synthese::env::Point  
	calculateSingleShiftedPoint (synthese::env::Point a, 
				     synthese::env::Point b, 
				     synthese::env::Point c, 
				     double distance) const;
    synthese::env::Point  
	calculateDoubleShiftedPoint (synthese::env::Point a, 
				     synthese::env::Point b, 
				     synthese::env::Point c, 
				     double incomingDistance, 
				     double outgoingDistance) const;
    
    
    
};
 
}
}

#endif

