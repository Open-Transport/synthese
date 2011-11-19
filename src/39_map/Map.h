
/** Map class header.
	@file Map.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_CARTO_MAP_H
#define SYNTHESE_CARTO_MAP_H

#include <iostream>


#include "Rectangle.h"
#include "DrawableLineIndex.h"


#include <set>
#include <map>
#include <vector>

namespace geos
{
	namespace geom
	{
		class Coordinate;
	}
}

namespace synthese
{

namespace map
{

    class DrawableLine;
    class DrawablePhysicalStop;
    class MapBackgroundManager;
    class PostscriptCanvas;

/** @ingroup m39 */
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
			       const geos::geom::Coordinate& point) const;

    std::pair<geos::geom::Coordinate, int>
	findMostSharedPoint (const DrawableLine* drawableLine,
			     const std::set<DrawableLine*>& exclusionList =
			     std::set<DrawableLine*> ()) const;

    DrawableLine* findMostSharedLine (const std::set<DrawableLine*>& drawableLines,
				      const std::set<DrawableLine*>& exclusionList) const;


    std::pair<const DrawableLine*, int>
	findLeftMostLine (const geos::geom::Coordinate& vertex,
			  const DrawableLine* reference,
			  const std::set<DrawableLine*>& lines) const;


    std::pair<const DrawableLine*, int>
	findRightMostLine (const geos::geom::Coordinate& vertex,
			   const DrawableLine* reference,
			   const std::set<DrawableLine*>& lines) const;


    void
	assignShiftFactors (const DrawableLine* reference,
			    const geos::geom::Coordinate& referencePoint,
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

    geos::geom::Coordinate toRealFrame (const geos::geom::Coordinate& p);
    geos::geom::Coordinate toOutputFrame (const geos::geom::Coordinate& p);

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

