#ifndef SYNTHESE_CARTO_RENDERER_H
#define SYNTHESE_CARTO_RENDERER_H


#include <iostream>
#include <string>
#include "RenderingConfig.h"
#include "01_util/Factorable.h"
#include <boost/filesystem/path.hpp>


namespace synthese
{
	namespace env
	{
		class Environment;
	}

	namespace carto
	{
		class Map;


		class Renderer : public util::Factorable
		{
		protected:

			RenderingConfig _config;

		public:

			static const std::string GHOSTSCRIPT_BIN;

			virtual ~Renderer ();

			virtual std::string render (const boost::filesystem::path& tempDir, 
				const std::string& filenamePrefix,
				const synthese::env::Environment* environment,
				synthese::carto::Map& map,
				const synthese::carto::RenderingConfig& config) = 0;
		    
		};

	}
}

#endif

