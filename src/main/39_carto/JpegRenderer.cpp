
#include "JpegRenderer.h"
#include "PostscriptRenderer.h"
#include <sstream>
#include "Map.h"
#include "01_util/Log.h"
#include "01_util/Exception.h"
#include <boost/filesystem/operations.hpp>

namespace synthese
{
	using namespace util;

	namespace carto
	{
		const std::string JpegRenderer::_factory_key = util::Factory<Renderer>::integrate<JpegRenderer>("jpeg");


		std::string JpegRenderer::render(const boost::filesystem::path& tempDir,  const std::string& filenamePrefix, const synthese::env::Environment* environment, synthese::carto::Map& map, const synthese::carto::RenderingConfig& config )
		{
			PostscriptRenderer psRenderer;
			psRenderer.render(tempDir, filenamePrefix, environment, map, config);

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
				throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
			}

			return resultFilename;

		}
	}
}