
#ifndef SYNTHESE_JpegRenderer_H__
#define SYNTHESE_JpegRenderer_H__


#include "Renderer.h"
#include "PostscriptCanvas.h"

#include "01_util/RGBColor.h"
#include "15_env/Point.h"

#include <iostream>
#include <string>
#include <vector>



namespace synthese
{


	namespace map
	{


		class JpegRenderer : public Renderer
		{
		public:

			//PostscriptRenderer (const RenderingConfig& config, std::ostream& output);
			virtual ~JpegRenderer ();

			std::string render(const boost::filesystem::path& tempDir, 
					   const std::string& filenamePrefix,
					   const synthese::env::Line::Registry& lines,
				synthese::map::Map& map,
				const synthese::map::RenderingConfig& config);


		};

	}
}

#endif // SYNTHESE_JpegRenderer_H__

