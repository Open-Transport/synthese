
/** MapInfoRenderer class header.
	@file MapInfoRenderer.h

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

#ifndef SYNTHESE_CARTO_MAPINFORENDERER_H
#define SYNTHESE_CARTO_MAPINFORENDERER_H

#include "Renderer.h"

#include "01_util/RGBColor.h"
#include "01_util/FactorableTemplate.h"

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>



namespace synthese
{


namespace map
{

    class DrawableLine;


	/** @ingroup m39 */
	class MapInfoRenderer : public util::FactorableTemplate<Renderer,MapInfoRenderer>
{
 private:
    static const std::string ZIP_BIN;

    boost::filesystem::path _zipOutput; //!< File used for MapInfo zip (containing MID/MIF) generation

 public:


    /** Constructor.
     * @param config Rendering configuration.
     * @param output Output stream
     */
//    MapInfoRenderer (const RenderingConfig& config,
//		     const boost::filesystem::path& zipOutput);

	 MapInfoRenderer();
    virtual ~MapInfoRenderer ();

    std::string render(const boost::filesystem::path& tempDir,
		       const std::string& filenamePrefix,
			   const util::Registry<pt::JourneyPattern>& lines,
		       synthese::map::Map& map,
		       const synthese::map::RenderingConfig& config);


};

}
}

#endif

