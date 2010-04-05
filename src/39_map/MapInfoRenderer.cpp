
/** MapInfoRenderer class implementation.
	@file MapInfoRenderer.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "MapInfoRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"

#include "Log.h"
#include "Conversion.h"
#include "PhysicalStop.h"

#include <boost/algorithm/string/replace.hpp>

#include "Point2D.h"

#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace geometry;
	using namespace map;
	using namespace pt;
	using namespace pt;
	

	namespace util
	{
		template<>
		const string FactorableTemplate<Renderer,MapInfoRenderer>::FACTORY_KEY("mapinfo");
	}
	

namespace map
{

const std::string MapInfoRenderer::ZIP_BIN ("zip");



MapInfoRenderer::~MapInfoRenderer()
{
}





std::string 
MapInfoRenderer::render(const boost::filesystem::path& tempDir, 
			const std::string& filenamePrefix,
			const Registry<Line>& lines,
			synthese::map::Map& map,
			const synthese::map::RenderingConfig& config)
{
	std::string resultFilename (filenamePrefix + ".zip");
	_zipOutput = tempDir / resultFilename;

	_config = config;



    std::string mifFilename (boost::replace_last_copy (_zipOutput.string (), 
						  ".zip", ".mif"));
    std::string midFilename (boost::replace_last_copy (_zipOutput.string (), 
						  ".zip", ".mid"));
			     
    const boost::filesystem::path mifFile (mifFilename, boost::filesystem::native);
    const boost::filesystem::path midFile (midFilename, boost::filesystem::native);

    std::ofstream mifof (mifFile.string ().c_str ());
    std::ofstream midof (midFile.string ().c_str ());

    mifof << "Version 300" << std::endl;
    mifof << "Charset \"WindowsLatin1\"" << std::endl;
    mifof << "Delimiter \",\"" << std::endl;
    mifof << "Index 52" << std::endl;
    mifof << "CoordSys Earth Projection 3, 1002, \"m\", 0, 46.8, 45.898918964419, 47.696014502038, 600000, 2200000 Bounds (-113967455.417, -106367759.649) (115167455.417, 122767151.185)" << std::endl; // this should never change...
    mifof << "Columns 1" << std::endl; 
    mifof << "  Libelle Char(200)" << std::endl;
    mifof << "Data" << std::endl << std::endl; 

    const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
         it != selectedLines.end () ; ++it) 
    {
	const DrawableLine* dbl = *it;
	const std::vector<const Point2D*>& points = dbl->getPoints ();


	
	int firstStopIndex = -1;
	// find first stop index
	for (int i=0; i<points.size ();++i)
	{
	    const PhysicalStop* stop = dynamic_cast<const PhysicalStop*> (points[i]);
	    if (stop == 0) continue;
	    firstStopIndex = i;
	    break;
	}

	if (firstStopIndex == -1) continue;

	
	while (true)
	{

	    // look for second stop index
	    int secondStopIndex = -1;


	    for (int i=firstStopIndex+1; i<points.size (); ++i)
	    {
		const PhysicalStop* stop = dynamic_cast<const PhysicalStop*> (points[i]);
		if (stop == 0) continue;
		secondStopIndex = i;
		break;
	    }
	    
	    if (secondStopIndex == -1) break;
	    
	    mifof << "Pline " << (secondStopIndex - firstStopIndex + 1) << std::endl;
	    
	    // Dump points between first stop index and second point index
	    for (int i=firstStopIndex; i<=secondStopIndex; ++i)
	    {
		mifof << points[i]->getX () << " " << points[i]->getY () << std::endl;
	    }
	    mifof << "    Pen (" 
		  << 3 << "," // pen width
		  << 2 << "," // pen pattern
		  << (((int) (dbl->getColor ().r * 65536) + 
		       ((int) dbl->getColor ().g * 256) + 
		       (int) dbl->getColor ().b)) // pen color
		  << ")" << std::endl;

	    midof << ((const PhysicalStop*) points[firstStopIndex])->getName ()
		  << " > " << ((const PhysicalStop*) points[secondStopIndex])->getName ()
		  << std::endl;
	    
	    firstStopIndex = secondStopIndex;
	}
    }


    midof.close ();
    mifof.close ();

    // Once everything generated, create the zip file
    std::stringstream zipcmd;
    zipcmd << ZIP_BIN << " " << _zipOutput.string () << " " 
	   << mifFile.string () << " " << midFile.string ();
    
    Log::GetInstance ().debug (zipcmd.str ());
    
    int ret = system (zipcmd.str ().c_str ());
    
    if (ret != 0)
    {
	throw synthese::util::Exception ("Error executing Zip (zip executable in path ?)");
    }
    

    return resultFilename;
}

MapInfoRenderer::MapInfoRenderer()
: FactorableTemplate<Renderer,MapInfoRenderer>()
{

}




/*

void 
MapInfoRenderer::renderLines (Map& map)
{
    const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
    
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
         it != selectedLines.end () ; ++it) {
		const DrawableLine* dbl = *it;
		dbl->prepare (map, _config.getSpacing ());
    }    
    
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
         it != selectedLines.end () ; ++it) 
	{
		const DrawableLine* dbl = *it;
		const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();

		// Shift them again on right and left of half-width to get the enveloppe.
		const std::vector<synthese::pt::Point2D> points1 =
		  dbl->calculateAbsoluteShiftedPoints (shiftedPoints, (_config.getBorderWidth () / 2));
		
		std::string href (_urlPattern);
		boost::replace_all (href, "$id", dbl->getLineId ());

		_output << "<area href='" << href << "' shape='poly' coords='";
		for (int i=0; i<points1.size (); ++i)
		{
			_output << (int) points1[i].getX () << "," << (int) (map.getHeight () - points1[i].getY ()) << ",";
		}

		std::vector<synthese::pt::Point2D> points2 = 
			dbl->calculateAbsoluteShiftedPoints (shiftedPoints, - (_config.getBorderWidth () / 2));

		std::reverse (points2.begin (), points2.end ());

		for (int i=0; i<points2.size (); ++i)
		{
			_output << (int) points2[i].getX () << "," << (int) (map.getHeight () - points2[i].getY ());
			if (i != points2.size ()-1) _output << ",";
		}
		_output << "'/>" << std::endl;

    }    



}





void 
MapInfoRenderer::renderPhysicalStops (Map& map)
{
    const std::set<DrawablePhysicalStop*>& selectedPhysicalStops = 
	map.getSelectedPhysicalStops ();

    for (std::set<DrawablePhysicalStop*>::const_iterator it = selectedPhysicalStops.begin ();
         it != selectedPhysicalStops.end () ; ++it) 
    {
	const DrawablePhysicalStop* dps = *it;

	std::string href (_urlPattern);
	boost::replace_all (href, "$id", Conversion::ToString (dps->getPhysicalStopId ()));

	_output << "<area href='" << href << "' shape='circle' coords='";
	_output << dps->getPoint ().getX ()  << "," 
		<< (int) (map.getHeight () - dps->getPoint ().getY()) << "," 
		<< _config.getLineWidth () << "'/>" << std::endl;
    }
}

*/





}
}
