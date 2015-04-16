
/** JpegRenderer class implementation.
	@file JpegRenderer.cpp

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

#include "JpegRenderer.h"
#include "PostscriptRenderer.h"
#include <sstream>
#include "Map.h"

#include "01_util/Log.h"
#include "Exception.h"

#include <boost/filesystem/operations.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace map;
	using namespace pt;


	namespace util
	{
		template<>
		const string FactorableTemplate<Renderer,JpegRenderer>::FACTORY_KEY("jpeg");
	}

	namespace map
	{


	    std::string JpegRenderer::render(const boost::filesystem::path& tempDir,
					     const std::string& filenamePrefix,
					     const Registry<JourneyPattern>& lines,
					     synthese::map::Map& map,
					     const synthese::map::RenderingConfig& config )
		{
			PostscriptRenderer psRenderer;
			psRenderer.render(tempDir, filenamePrefix, lines, map, config);

			std::string psFilename = filenamePrefix + ".ps";
			const boost::filesystem::path psFile (tempDir / psFilename);

			std::string resultFilename (filenamePrefix + ".jpg");
			const boost::filesystem::path jpegFile (tempDir / resultFilename);

			// Convert the ps file to jpeg with ghostscript
			std::stringstream gscmd;
			gscmd << GHOSTSCRIPT_BIN << " -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=jpeg -dJPEGQ=50 -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -g"
				<< map.getWidth () << "x" << map.getHeight ()
				<< " -sOutputFile=" << jpegFile.string () << " " << psFile.string ();

			Log::GetInstance ().debug (gscmd.str ());

			int ret = system (gscmd.str ().c_str ());

			boost::filesystem::remove (psFile);

			if (ret != 0)
			{
				throw synthese::Exception ("Error executing GhostScript (gs executable in path ?)");
			}

			return resultFilename;

		}

		JpegRenderer::~JpegRenderer()
		{

		}

		JpegRenderer::JpegRenderer()
			: FactorableTemplate<Renderer,JpegRenderer>()
		{

		}
	}
}
