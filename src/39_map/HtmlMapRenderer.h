
/** HtmlMapRenderer class header.
	@file HtmlMapRenderer.h

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

#ifndef SYNTHESE_CARTO_HTMLMAPRENDERER_H
#define SYNTHESE_CARTO_HTMLMAPRENDERER_H

#include "Renderer.h"
#include "Registry.h"
#include "RGBColor.h"
#include "FactorableTemplate.h"

#include <iostream>
#include <string>
#include <vector>



namespace synthese
{
	namespace pt
	{
		class Environment;
	}


	namespace map
	{

		class DrawableLine;


		/** @ingroup m39 */
		class HtmlMapRenderer : public util::FactorableTemplate<Renderer,HtmlMapRenderer>
		{
		private:
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

			std::string render (const boost::filesystem::path& tempDir,
					    const std::string& filenamePrefix,
						const util::Registry<pt::JourneyPattern>& lines,
						synthese::map::Map& map,
					    const synthese::map::RenderingConfig& config);

		private:

			void renderLines(
				std::ostream& output,
				const util::Registry<pt::JourneyPattern>& lines,
				Map& map
			);

			void renderPhysicalStops (std::ostream& output, Map& map);


		};
	}
}

#endif

