

/** GeographyModule class header.
	@file GeographyModule.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_GeographyModule_H__
#define SYNTHESE_GeographyModule_H__

#include "FactorableTemplate.h"
#include "ModuleClass.h"
#include "LexicalMatcher.h"
#include "City.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	/**	@defgroup m09 09 Geography
		@ingroup m0
		
		(Module documentation)
		
		@{
	*/

	/** 09 Geography Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace geography
	{
		/** 09 Geography Module class.
			@author Hugues Romain
			@date 2008
		*/
		class GeographyModule:
			public util::FactorableTemplate<util::ModuleClass, GeographyModule>
		{
		public:
			typedef lexmatcher::LexicalMatcher<City*> CitiesMatcher;
			typedef std::vector<CitiesMatcher::Content> CityList;

		private:

			static CitiesMatcher _citiesMatcher;
			static CitiesMatcher _citiesT9Matcher;

		public:
			/** Initialization of the 09 Geography module after the automatic database loads.
				@author Hugues Romain
				@date 2008
			*/			
			void initialize();
			
			virtual std::string getName() const;


			//////////////////////////////////////////////////////////////////////////
			/// Finds the best place corresponding to a pair of texts : city name and
			/// place name.
			/// @param city name of the city
			/// @param place name of the place within the city
			/// @return the best choice
			/// Even if no place name really corresponds to the entered texts,
			/// a result is ever returned.
			static const Place* FetchPlace(
				const std::string& city,
				const std::string& place
			);



			/** Find the best matches in the city list comparing to a text entry.
				@param fuzzyName The text entry to compare
				@param nbMatches The number of matches to return
				@param t9 true indicates that the text entry follows the t9 format (optional : default = false)
				@return synthese::env::CityList The list of results
				@author Hugues Romain
				@date 2008
			*/
			static CityList GuessCity(
				const std::string& fuzzyName
				, int nbMatches = 10
				, bool t9 = false
			);

			static void AddToCitiesMatchers(
				CitiesMatcher::Content city
			);

			static void RemoveFromCitiesMatchers(
				CitiesMatcher::Content city
			);

		};
	}
	/** @} */
}

#endif // SYNTHESE_GeographyModule_H__
