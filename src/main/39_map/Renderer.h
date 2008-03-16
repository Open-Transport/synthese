#ifndef SYNTHESE_CARTO_RENDERER_H
#define SYNTHESE_CARTO_RENDERER_H


#include <iostream>
#include <string>

#include "RenderingConfig.h"
#include "15_env/Line.h"

#include "01_util/Factorable.h"

#include <boost/filesystem/path.hpp>


namespace synthese
{

	namespace map
	{
		class Map;


		class Renderer : public util::Factorable<Renderer>
		{
		protected:

			RenderingConfig _config;

		public:

			static const std::string GHOSTSCRIPT_BIN;

			virtual ~Renderer ();

			virtual std::string render (const boost::filesystem::path& tempDir, 
						    const std::string& filenamePrefix,
						    const synthese::env::Line::Registry& lines,
						    synthese::map::Map& map,
						    const synthese::map::RenderingConfig& config) = 0;
		    
		};

	}
}

#endif

