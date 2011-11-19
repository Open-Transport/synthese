
/// Renderer class header.
///	@file Renderer.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_CARTO_RENDERER_H
#define SYNTHESE_CARTO_RENDERER_H


#include <iostream>
#include <string>

#include "Registry.h"
#include "RenderingConfig.h"
#include "JourneyPattern.hpp"

#include "FactoryBase.h"

#include <boost/filesystem/path.hpp>


namespace synthese
{

	namespace map
	{
		class Map;


		/// Map renderer base class (factory).
		/// @ingroup m39
		class Renderer
		:	public util::FactoryBase<Renderer>
		{
		public:

		protected:

			RenderingConfig _config;

		public:

			static const std::string GHOSTSCRIPT_BIN;

			virtual ~Renderer ();

			virtual std::string render (const boost::filesystem::path& tempDir,
						    const std::string& filenamePrefix,
							const util::Registry<pt::JourneyPattern>& lines,
						    synthese::map::Map& map,
						    const synthese::map::RenderingConfig& config) = 0;

		};

	}
}

#endif

