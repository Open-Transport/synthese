
/** PublicBikingModule class header.
	@file PublicBikingModule.h
	@author Camille Hue
	@date 2015

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

#ifndef SYNTHESE_PublicBikingModule_H__
#define SYNTHESE_PublicBikingModule_H__

#include "GeographyModule.h"
#include "GraphModuleTemplate.h"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 65 Public biking module namespace.
	///	@author Camille Hue
	///	@date 2015
	/// @ingroup m64
	namespace public_biking
	{
		class PublicBikeStation;

		//////////////////////////////////////////////////////////////////////////
		/// 65 public_biking Module class.
		///	@author Camille Hue
		///	@date 2015
		class PublicBikingModule:
			public graph::GraphModuleTemplate<PublicBikingModule>
		{
		public:
			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<PublicBikeStation> > GeneralPublicBikeStationsMatcher;

		private:
			static GeneralPublicBikeStationsMatcher _generalPublicBikeStationsMatcher;

		public:
			static GeneralPublicBikeStationsMatcher& GetGeneralPublicBikeStationsMatcher(){ return _generalPublicBikeStationsMatcher; }

		private:


		public:
			/** Initialization of the 65 public_biking module after the automatic database loads.
				@author Camille Hue
				@date 2015
			*/
			//void initialize();

		};
	}
	/** @} */
}

#endif // SYNTHESE_PublicBikingModule_H__
