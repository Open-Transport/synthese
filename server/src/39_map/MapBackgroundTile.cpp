
/** MapBackgroundTile class implementation.
	@file MapBackgroundTile.cpp

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

#include "MapBackgroundTile.h"

#include "Map.h"
#include "PostscriptCanvas.h"

#include <geos/geom/Coordinate.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace geos::geom;


namespace synthese
{

namespace map
{


MapBackgroundTile::MapBackgroundTile(const boost::filesystem::path& path,
                                     int pixelWidth, int pixelHeight,
                                     double topLeftX, double topLeftY,
                                     double bottomRightX, double bottomRightY)
: _path (path)
, _pixelWidth (pixelWidth)
, _pixelHeight (pixelHeight)
, _topLeftX (topLeftX)
, _topLeftY (topLeftY)
, _bottomRightX (bottomRightX)
, _bottomRightY (bottomRightY)
{

}

MapBackgroundTile::~MapBackgroundTile()
{
}


bool
MapBackgroundTile::operator< (const MapBackgroundTile& rhs) const
{
    if (_topLeftY != rhs._topLeftY) {
        return _topLeftY < rhs._topLeftY;
    }
    return (_topLeftX < rhs._topLeftX);
}

double
MapBackgroundTile::getWidth () const
{
    return _bottomRightX - _topLeftX;
}


double
MapBackgroundTile::getHeight () const
{
    return _topLeftY - _bottomRightY;
}




void
MapBackgroundTile::preDraw (Map& map, PostscriptCanvas& canvas) const
{

}



void
MapBackgroundTile::postDraw (Map& map, PostscriptCanvas& canvas) const
{

}



void
MapBackgroundTile::draw (Map& map, PostscriptCanvas& canvas) const
{

    Coordinate tl = map.toOutputFrame (Coordinate (_topLeftX, _topLeftY));
    Coordinate br = map.toOutputFrame (Coordinate (_bottomRightX, _bottomRightY));

    double outputWidth = br.x - tl.x;
    double outputHeight = tl.y - br.y;


    double translateX = tl.x;
    double translateY = br.y; //map.getHeight () - tl.getY ();

    ostream& os = canvas.getOutput ();

    // Dump the common procedure for drawing an image

    os << "gsave 100 dict begin " << endl;
    os << translateX << " " << translateY << " translate" << endl;   // TODO !!
    os << outputWidth << " " << outputHeight << " scale" << endl; // TO check!!
    os <<  "/bwproc { rgbproc dup length 3 idiv string 0 3 0 5 -1 roll {" << endl;
    os << "add 2 1 roll 1 sub dup 0 eq { pop 3 idiv 3 -1 roll dup 4 -1 roll" << endl;
    os << "dup 3 1 roll 5 -1 roll put 1 add 3 0 } { 2 1 roll } ifelse" << endl;
    os << "} forall pop pop pop } def /colorimage where {pop} {" << endl;
    os << "/colorimage {pop pop /rgbproc exch def {bwproc} image} bind def" << endl;
    os << "} ifelse" << endl;
    os << "/scanLine " << _pixelWidth*3 << " string def" << endl;
    os << _pixelWidth << " " << _pixelHeight << " 8" << endl;
    os << "[" << _pixelWidth << " 0 0 " << -_pixelWidth << " 0 " << _pixelWidth << "]" << endl;
    os << "{currentfile scanLine readhexstring pop} bind" << endl;
    os << "false 3 colorimage" << endl;

    // Dump the image data
    ifstream ifs (_path.string().c_str ());
    char buf[4096];
    while (ifs) {
        ifs.getline (buf, 4096);
        os << buf ;
    }
    os << endl;

    os << "end grestore" << endl;

}


}
}

