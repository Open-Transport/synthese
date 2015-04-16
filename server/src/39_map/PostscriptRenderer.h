
/** PostscriptRenderer class header.
	@file PostscriptRenderer.h

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

#ifndef SYNTHESE_CARTO_POSTSCRIPTRENDERER_H
#define SYNTHESE_CARTO_POSTSCRIPTRENDERER_H

#include "Renderer.h"
#include "PostscriptCanvas.h"

#include "RGBColor.h"
#include "FactorableTemplate.h"

#include <iostream>
#include <string>
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


	/** @ingroup m39 */
	class PostscriptRenderer : public util::FactorableTemplate<Renderer,PostscriptRenderer>
{
 public:

    PostscriptRenderer ();
    virtual ~PostscriptRenderer ();

	std::string render(const boost::filesystem::path& tempDir,
		const std::string& filenamePrefix,
		const util::Registry<pt::JourneyPattern>& lines,
		synthese::map::Map& map,
		const synthese::map::RenderingConfig& config);


 private:

    void renderBackground (PostscriptCanvas& _canvas, Map& map);
    void renderLines (PostscriptCanvas& _canvas,Map& map);
    void renderPhysicalStops (PostscriptCanvas& _canvas,Map& map);

    void doDrawCurvedLine (PostscriptCanvas& _canvas,const DrawableLine* dbl);

	void doDrawTriangleArrow (PostscriptCanvas& _canvas,const geos::geom::Coordinate& point,
                              double angle);

	void doDrawSquareStop (PostscriptCanvas& _canvas,const geos::geom::Coordinate& point,
                           double angle);

	void doDrawSquareTerminus (PostscriptCanvas& _canvas,const geos::geom::Coordinate& point,
			       double angle);

};

}
}

#endif

