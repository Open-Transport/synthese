
/** MapBackgroundTile class header.
	@file MapBackgroundTile.h

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

#ifndef SYNTHESE_CARTO_MAPBACKGROUNDTILE_H
#define SYNTHESE_CARTO_MAPBACKGROUNDTILE_H

#include <boost/filesystem/path.hpp>
#include "Drawable.h"


namespace synthese
{
namespace map
{


class Map;

/** @ingroup m39 */
class MapBackgroundTile : public Drawable
{
private:

    boost::filesystem::path _path;
    int _pixelWidth;
    int _pixelHeight;
    double _topLeftX;
    double _topLeftY;
    double _bottomRightX;
    double _bottomRightY;

public:


    MapBackgroundTile(const boost::filesystem::path& path,
                      int pixelWidth, int pixelHeight,
                      double topLeftX, double topLeftY,
                      double bottomRightX, double bottomRightY);

    virtual ~MapBackgroundTile();


    const boost::filesystem::path& getPath () const { return _path; }
    double getTopLeftX () const { return _topLeftX; }
    double getTopLeftY () const { return _topLeftY; }

    double getWidth () const;
    double getHeight () const;

    int getPixelWidth () const { return _pixelWidth; }
    int getPixelHeight () const { return _pixelHeight; }

    virtual void preDraw (Map& map, PostscriptCanvas& canvas) const;

    virtual void draw (Map& map, PostscriptCanvas& canvas) const;

    virtual void postDraw (Map& map, PostscriptCanvas& canvas) const;

    bool operator< (const MapBackgroundTile& rhs) const;


};

}
}

#endif

