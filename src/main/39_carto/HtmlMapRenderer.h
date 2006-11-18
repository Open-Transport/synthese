#ifndef SYNTHESE_CARTO_HTMLMAPRENDERER_H
#define SYNTHESE_CARTO_HTMLMAPRENDERER_H

#include "Renderer.h"

#include "01_util/RGBColor.h"
#include "15_env/Point.h"

#include <iostream>
#include <string>
#include <vector>



namespace synthese
{
	namespace env
	{
		class Environment;
	}


	namespace carto
	{

		class DrawableLine;


		class HtmlMapRenderer : public Renderer
		{
		private:
			const env::Environment* _environment;
			std::string _urlPattern; //!< URL pattern ($id is replaced by the proper object id)
			std::string _mapImgFilename; //!< Filename of the img used in generated HTML map
			
		public:


			/** Constructor.
			* @param config Rendering configuration.
			* @param urlPattern URL to be used in map output. In this URL, $id, will be replaced by
			object id corresponding to the area (line, or physical stop).
			* @param mapImgFilename Filename (without path) of the img used in generated HTML map
			* @param output Output stream
			*/
			HtmlMapRenderer ();

			virtual ~HtmlMapRenderer ();

			std::string render(const boost::filesystem::path& tempDir, 
				const std::string& filenamePrefix,
				const synthese::env::Environment* environment,
				synthese::carto::Map& map,
				const synthese::carto::RenderingConfig& config);
			
		private:

			void renderLines (std::ostream& output, Map& map);
			void renderPhysicalStops (std::ostream& output, Map& map);

			
		};
	}
}

#endif
