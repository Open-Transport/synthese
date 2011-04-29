
/** JpegRenderer class header.
	@file JpegRenderer.h

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

#ifndef SYNTHESE_JpegRenderer_H__
#define SYNTHESE_JpegRenderer_H__


#include "Renderer.h"
#include "PostscriptCanvas.h"
#include "Registry.h"
#include "RGBColor.h"
#include "FactorableTemplate.h"

#include <iostream>
#include <string>
#include <vector>



namespace synthese
{


	namespace map
	{


		class JpegRenderer : public util::FactorableTemplate<Renderer,JpegRenderer>
		{
		public:

			JpegRenderer();
			virtual ~JpegRenderer ();

			std::string render(const boost::filesystem::path& tempDir,
					   const std::string& filenamePrefix,
					   const util::Registry<pt::JourneyPattern>& lines,
				synthese::map::Map& map,
				const synthese::map::RenderingConfig& config);


		};

	}
}

#endif // SYNTHESE_JpegRenderer_H__

